/*
	MixSets by Junior_Djjr and entire GTA 3D Era community
*/
#include "MixSetsIII.h"
#include "ReadIni.h"
#include "Common.h"

// Game
#include "extensions/ScriptCommands.h"
#include "CWeather.h"
#include "CTheScripts.h"
#include "CTimer.h"
#include "CMessages.h"

// Other
#include "..\injector\assembly.hpp"
#include "IniReader/IniReader.h"
#include "TestCheat.h"

using namespace plugin;
using namespace injector;
using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////

class MixSetsIII mixSets;

MixSetsIII::MixSetsIII()
{
	lg.open("MixSets.log", fstream::out | fstream::trunc);
	lg << "v1.0.0" << "\n";
	lg.flush();


	// -- Default values
	bEnabled = false;

	Read = false;

	bProcessOnceOnScripts = false;
	bProcessOnceAfterIntro = false;
	bNoCLEO = true;

	numOldCfgNotFound = 0;

	zero = 0.0;

	strcpy_s(G_NoMoneyZeros_Pos, "$%d");
	strcpy_s(G_NoMoneyZeros_Neg, "-$%d");

	// -- End of Default values


	if (GetGameVersion() != GAME_10EN)
	{
		lg << "ERROR: Game version not supported. Download GTA III Crack 1.0." << "\n\n";
		lg.flush();
		MessageBoxA(0, "Game version not supported. Download GTA III Crack 1.0.", "MixSets", 0);
		return;
	}

	Events::initRwEvent += []
	{
		if (!GetModuleHandleA("CLEO.asi")) {
			//lg << "ERROR: CLEO isn't installed. It's required for some MixSets features." << "\n\n";
			bNoCLEO = true;
		}
		else bNoCLEO = false;

		if (GetModuleHandleA("III.VC.SA.LimitAdjuster.asi")) {
			lg << "Open Limit Adjuster = true" << "\n";
			bOLA = true;
		}
		else bOLA = false;

		ReadIni_BeforeFirstFrame();

		if (bEnabled) {
			if (lang == languages::PT)
				lg << "\n" << "Terminado de ler o ini na inicialização do jogo" << "\n\n";
			else
				lg << "\n" << "Done read ini on game init" << "\n\n";
		}

		lg.flush();

		Read = true;
	};

	Events::initScriptsEvent += []
	{
		bProcessOnceOnScripts = true;
		bProcessOnceAfterIntro = true;
	};


	Events::processScriptsEvent.before += []
	{
		if (bEnabled)
		{
			if (Read)
			{
				ReadIni();

				if (lang == languages::PT)
					lg << "\n" << "Terminado de ler o ini no primeiro frame." << "\n\n";
				else
					lg << "\n" << "Done read ini on first frame" << "\n\n";
				lg.flush();

				Read = false;
			}

			//////////////////////////////////////////////

			//CPed* playerPed = FindPlayerPed();

			if (G_HowManyMinsInDay > 0)
			{
				WriteMemory<uint32_t>(0x8F2C64, G_HowManyMinsInDay, false);
			}

			if (G_FreezeWeather >= 0)
				CWeather::ForceWeatherNow(G_FreezeWeather);
		}

		if (bProcessOnceAfterIntro && *(CTheScripts::ScriptSpace + (24 * 4)) == 1 /* intro passed */)
		{
			bProcessOnceAfterIntro = false;
			ShowModMessages(); // second time, just to make sure
			if (bEnabled)
			{
			}
		}

		if (G_ReloadCommand.length() > 0)
		{
			if (TestCheat(&G_ReloadCommand[0]))
			{
				if (lang == languages::PT)
				{
					lg << "Recarregando..." << "\n\n";
					CMessages::AddMessageJumpQ("'MixSets.ini' recarregando.", 1000, 1);
				}
				else
				{
					lg << "Reloading..." << "\n\n";
					CMessages::AddMessageJumpQ("'MixSets.ini' reloading.", 1000, 1);
				}
				lg.flush();

				bReloading = true;

				MixSetsIII::ReadIni_BeforeFirstFrame();
				MixSetsIII::ReadIni();
				bProcessOnceAfterIntro = true;
				bProcessOnceOnScripts = true;

				bReloading = false;
			}
		}
	};

}


///////////////////////////////////////////////////////////////////////////////////////////////////


void MixSetsIII::ShowModMessages()
{
	if (MixSetsIII::bIniFailed)
	{
		CMessages::AddMessageJumpQ("~r~ERROR: MixSets.ini not found - MixSets.ini nao encontrado", 6000, false);
	}
	else
	{
		if (MixSetsIII::bVersionFailed)
		{
			if (MixSetsIII::lang == languages::PT)
				CMessages::AddMessageJumpQ("~r~MixSets: O executavel do seu jogo nao e compativel. Use Crack 1.0 US.", 10000, false);
			else
				CMessages::AddMessageJumpQ("~r~MixSets: Your game executable isn't compatible. Use Crack 1.0 US.", 10000, false);
		}
		else
		{
			if (MixSetsIII::bReadOldINI)
			{
				if (MixSetsIII::bErrorRename)
				{
					if (MixSetsIII::lang == languages::PT)
						CMessages::AddMessageJumpQ("~r~As configuracoes do 'MixSets old.ini' foram movidas, mas houve um erro ao renomea-lo. Leia o 'MixSets.log'.", 10000, false);
					else
						CMessages::AddMessageJumpQ("~r~The 'MixSets old.ini' settings was moved, but there is an error when renaming it. Read the 'MixSets.log'.", 10000, false);
				}
				else
				{
					if (MixSetsIII::numOldCfgNotFound > 0)
					{
						if (MixSetsIII::numOldCfgNotFound > 1)
						{
							if (MixSetsIII::lang == languages::PT)
								CMessages::AddMessageJumpQWithNumber("~y~As configuracoes do 'MixSets old.ini' foram movidas. Ha ~1~ avisos, leia o 'MixSets.log'.", 8000, false, MixSetsIII::numOldCfgNotFound, 0, 0, 0, 0, 0);
							else
								CMessages::AddMessageJumpQWithNumber("~y~The 'MixSets old.ini' settings was moved. There is ~1~ warnings, read the 'MixSets.log'.", 8000, false, MixSetsIII::numOldCfgNotFound, 0, 0, 0, 0, 0);
						}
						else
						{
							if (MixSetsIII::lang == languages::PT)
								CMessages::AddMessageJumpQWithNumber("~y~As configuracoes do 'MixSets old.ini' foram movidas. Ha ~1~ aviso, leia o 'MixSets.log'.", 8000, false, MixSetsIII::numOldCfgNotFound, 0, 0, 0, 0, 0);
							else
								CMessages::AddMessageJumpQWithNumber("~y~The 'MixSets old.ini' settings was moved. There is ~1~ warning, read the 'MixSets.log'.", 8000, false, MixSetsIII::numOldCfgNotFound, 0, 0, 0, 0, 0);
						}
					}
					else
					{
						if (MixSetsIII::lang == languages::PT)
							CMessages::AddMessageJumpQ("~y~As configuracoes do 'MixSets old.ini' foram movidas com sucesso.", 6000, false);
						else
							CMessages::AddMessageJumpQ("~y~As configuracoes do 'MixSets old.ini' foram movidas com sucesso.", 6000, false);
					}
				}
			}
		}
	}
}

void MixSetsIII::ProcessPriority()
{
	HANDLE processHandle = GetCurrentProcess();
	int priority = 0;
	switch (MixSetsIII::G_ProcessPriority)
	{
	case 1:
		priority = IDLE_PRIORITY_CLASS;
		break;
	case 2:
		priority = BELOW_NORMAL_PRIORITY_CLASS;
		break;
	case 3:
		priority = NORMAL_PRIORITY_CLASS;
		break;
	case 4:
		priority = ABOVE_NORMAL_PRIORITY_CLASS;
		break;
	case 5:
		priority = HIGH_PRIORITY_CLASS;
		break;
	case 6:
		priority = REALTIME_PRIORITY_CLASS;
		break;
	default:
		priority = NORMAL_PRIORITY_CLASS;
		break;
	}
	SetPriorityClass(processHandle, priority);
}

void MixSetsIII::SetIdlePriority()
{
	HANDLE processHandle = GetCurrentProcess();
	SetPriorityClass(processHandle, IDLE_PRIORITY_CLASS);
}


///////////////////////////////////////////////////////////////////////////////////////////////////

void MixSetsIII::ReadOldINI(CIniReader ini, fstream* lg, string section, string key)
{
	MixSetsIII::bParsePreserveComments = true;

	CIniReader iniold("MixSets old.ini");
	string oldIniLine = iniold.ReadString(section, key, "");

	if (oldIniLine.length() <= 0)
	{
		if (MixSetsIII::lang == languages::PT)
			*lg << "Aviso: " << section << ": " << key << " não encontrado no 'MixSets old.ini'. Se não é função nova, então o nome foi alterado na nova versão. Verifique.\n";
		else
			*lg << "Warning: " << section << ": " << key << " not found in 'MixSets old.ini'. If isn't new feature, then the name has changed in new version. Check it.\n";

		MixSetsIII::bParsePreserveComments = false;

		MixSetsIII::numOldCfgNotFound++;
		return;
	}

	CIniReader ininew("MixSets.ini");

	string newIniLine = ininew.ReadString(section, key, "");

	MixSetsIII::bParsePreserveComments = false;

	int newIniLineLen = newIniLine.find_first_of(" #");
	int oldIniLineLen = oldIniLine.find_first_of(" #");

	newIniLine.replace(0, newIniLineLen, oldIniLine, 0, oldIniLineLen);

	int charsRemoved = (oldIniLineLen - newIniLineLen);

	if (charsRemoved < 0)
	{
		while (charsRemoved < 0)
		{
			newIniLine.insert(oldIniLineLen, " ");
			charsRemoved++;
		}
	}
	else
	{
		while (charsRemoved > 0)
		{
			if (&newIniLine[oldIniLineLen] == "#") break;
			newIniLine.replace(oldIniLineLen, 1, "");
			charsRemoved--;
		}
	}

	newIniLine.insert(0, " ");

	ininew.WriteString(section, key, newIniLine, false);
}

inline bool MixSetsIII::FileExists(const std::string& name)
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool MixSetsIII::ReadIniFloat(CIniReader ini, fstream* lg, string section, string key, float* f)
{
	if (MixSetsIII::bReadOldINI) ReadOldINI(ini, lg, section, key);
	*f = ini.ReadFloat(section, key, -1);
	if (*f != -1)
	{
		*lg << section << ": " << key << " = " << fixed << *f << "\n";
		return true;
	}
	else return false;
}

bool MixSetsIII::ReadIniInt(CIniReader ini, fstream* lg, string section, string key, int* i)
{
	if (MixSetsIII::bReadOldINI) ReadOldINI(ini, lg, section, key);
	*i = ini.ReadInteger(section, key, -1);
	if (*i != -1)
	{
		*lg << section << ": " << key << " = " << *i << "\n";
		return true;
	}
	else return false;
}

bool MixSetsIII::ReadIniBool(CIniReader ini, fstream* lg, string section, string key)
{
	if (MixSetsIII::bReadOldINI) ReadOldINI(ini, lg, section, key);
	//bool b = ini.ReadBoolean(section, key, 0);
	bool b = ini.ReadInteger(section, key, 0) == 1;
	if (b == true)
	{
		*lg << section << ": " << key << " = true \n";
		return true;
	}
	else return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void MixSetsIII::IncreaseMemoryValueIfValid(uintptr_t address, int32_t value, uint8_t validation, bool vp)
{
	if (ReadMemory<uint8_t>(address - 1, vp) != validation) return;
	if (ReadMemory<int32_t>(address, vp) < value) WriteMemory<int32_t>(address, value, vp);
}
void MixSetsIII::IncreaseMemoryValueIfValid_Byte(uintptr_t address, int8_t value, uint8_t validation, bool vp)
{
	if (ReadMemory<uint8_t>(address - 1, vp) != validation) return;
	if (ReadMemory<int8_t>(address, vp) < value) WriteMemory<int8_t>(address, value, vp);
}


///////////////////////////////////////////////////////////////////////////////////////////////////


void MixSetsIII::asm_fmul(float f)
{
	__asm {fmul f}
}
