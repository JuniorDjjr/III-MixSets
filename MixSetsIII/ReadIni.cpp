#include "Common.h"
#include "MixSetsIII.h"
#include "..\injector\assembly.hpp"
#include <game_III\common.h>
#include "CGame.h"
#include "CGeneral.h"
#include <filesystem>
#include "CWorld.h"
#include "CMenuManager.h"
#include "CWeather.h"
#include "eAnimations.h"

using namespace plugin;
using namespace injector;
using namespace std;

void MixSetsIII::ReadIni_BeforeFirstFrame()
{
	int i = 0;
	float f = 0.0f;

	bVersionFailed = false;
	numOldCfgNotFound = 0;

	CIniReader ini("MixSets.ini");


	if (ini.data.size() <= 0) {
		lg << "\nERROR: MixSets.ini not found - MixSets.ini não encontrado \n";
		bIniFailed = true;
		return;
	} bIniFailed = false;


	CIniReader iniold("MixSets old.ini");
	if (iniold.data.size() <= 0)
	{
		bReadOldINI = false;
	}
	else {
		bReadOldINI = true;
		if (lang == languages::PT)
			lg << "\n'MixSets old.ini' encontrado. As configurações serão movidas para o 'MixSets.ini'.\n\n";
		else
			lg << "\n'MixSets old.ini' found. The settings will be moved to 'MixSets.ini'.\n\n";
	}


	// -- Mod
	if (ReadIniInt(ini, &lg, "Mod", "Language", &i)) {
		switch (i)
		{
		case 1:
			lang = languages::PT;
			break;
		case 2:
			lang = languages::EN;
			break;
		default:
			break;
		}
	}
	else {
		lang = languages::EN;
		lg << "Language not read. Set to english\n";
	}


	if (ReadIniBool(ini, &lg, "Mod", "Enabled")) {
		bEnabled = true;
	}
	else {
		bEnabled = false;
		if (lang == languages::PT)
			lg << "Desativado" << "\n\n";
		else
			lg << "Disabled" << "\n\n";
		lg.flush();
		return;
	}

	gameVersion = GetGameVersion();

	if (gameVersion != GAME_10EN)
	{
		if (lang == languages::PT)
			lg << "\nERROR: O executável do seu jogo não é compatível. Use Crack 1.0.\n";
		else
			lg << "\nERROR: Your game executable isn't compatible. Use Crack 1.0.\n";
		bVersionFailed = true;
		bEnabled = false;
		return;
	} bVersionFailed = false;


	if (ReadIniBool(ini, &lg, "Mod", "NoDensities")) {
		G_NoDensities = true;
	}
	else G_NoDensities = false;


	G_ReloadCommand = ini.ReadString("Mod", "ReloadCommand", "");


	// -- System
	if (ReadIniInt(ini, &lg, "System", "FPSlimit", &i)) {
		WriteMemory<uint8_t>(0x8F4374, i, false);
		G_FPSlimit = i;
	}

	if (ReadIniInt(ini, &lg, "System", "FPSlimit", &i)) {
		G_ProcessPriority = i;
		ProcessPriority();
	}


	// -- Gameplay

	if (ReadIniBool(ini, &lg, "Graphics", "NoCoronas")) {
		MakeNOP(0x48E0B8, 5);
		MakeNOP(0x48C9D7, 5);
		MakeNOP(0x48E03F, 5);
	}


	// -- Densities
	if (!G_NoDensities) {

	}

	// -- World
	if (ReadIniInt(ini, &lg, "World", "HowManyMinsInDay", &i)) {
		i *= 41.666666667f;
		G_HowManyMinsInDay = i;
		//WriteMemory<uint32_t>(0x8F2C64, i, false); // on MixSets.cpp
		WriteMemory<uint32_t>(0x48C289+1, i, true);
		WriteMemory<uint32_t>(0x48C584+1, i, true);
	}

	lg.flush();
}


///////////////////////////////////////////////////////////////////////////////////////////////////


void MixSetsIII::ReadIni()
{
	if (!bEnabled) return;

	int i;
	float f;

	CIniReader ini("MixSets.ini");


	CIniReader iniold("MixSets old.ini");
	if (iniold.data.size() <= 0)
		bReadOldINI = false;
	else
		bReadOldINI = true;


	// -- System
	if (ReadIniFloat(ini, &lg, "System", "StreamMemoryMult", &f)) {
		WriteMemory<float>(0x5EC1C4, f, false);
	}

	if (ReadIniInt(ini, &lg, "System", "FPSlimit", &i)) {
		WriteMemory<uint8_t>(0x8F4374, i, false);
		G_FPSlimit = i;
	}
	else G_FPSlimit = -1;



	// -- Graphics

	if (ReadIniBool(ini, &lg, "Graphics", "NoPoleShadows")) {
		WriteMemory<uint8_t>(0x513E10, 0xC3, true);
	}

	if (ReadIniBool(ini, &lg, "Graphics", "NoRubbish")) {
		MakeNOP(0x48E0B3, 5);
		MakeNOP(0x48C998, 5);
	}

	if (ReadIniBool(ini, &lg, "Graphics", "NoRubbish")) {
		MakeNOP(0x518DC5, 5);
	}

	if (ReadIniBool(ini, &lg, "Graphics", "NoFakePlanes")) {
		MakeNOP(0x48C904, 5);
	}
	
	if (ReadIniBool(ini, &lg, "Graphics", "NoBrightLights")) {
		MakeNOP(0x518DCA, 5);
	}
	

	if (ReadIniFloat(ini, &lg, "Graphics", "RainGroundSplashNum", &f)) {//3.0
		G_RainGroundSplashNum = f;
		WriteMemory<float*>(0x523B14 + 2, &G_RainGroundSplashNum, true);
	}

	if (ReadIniBool(ini, &lg, "Graphics", "NoSkyStuff")) {
		MakeNOP(0x48E030, 5, true);
		MakeNOP(0x48D110, 5, true);
	}

	if (ReadIniBool(ini, &lg, "Graphics", "NoHorizonClouds")) {
		MakeNOP(0x48D110, 5, true);
	}

	if (ReadIniBool(ini, &lg, "Graphics", "NoSun")) {
		MakeNOP(0x48C9D2, 5, true);
	}

	if (ReadIniFloat(ini, &lg, "Graphics", "SunSize", &f)) { //25.0
		//G_SunSize = f;
		//WriteMemory<float*>(0x6FC6EA, &G_SunSize, true);
		WriteMemory<float>(0x5FB27C, f, false);
	}

	if (ReadIniFloat(ini, &lg, "Graphics", "CloudRotationSpeed", &f)) {
		WriteMemory<float>(0x5FAEA0, f, false);
	}


	// -- Gameplay

	if (ReadIniBool(ini, &lg, "Gameplay", "NoCheats")) {
		MakeJMP(0x49248E, 0x492715);
	}

	if (ReadIniFloat(ini, &lg, "Gameplay", "WheelTurnSpeed", &f)) {
		G_WheelTurnSpeed = f;
		WriteMemory<float*>(0x53B85A + 2, &G_WheelTurnSpeed, false);
		WriteMemory<float*>(0x53B8B4 + 2, &G_WheelTurnSpeed, false);
	}

	if (ReadIniBool(ini, &lg, "Gameplay", "NoWayForSiren")) {
		MakeNOP(0x53481D, 5, true);
	}

	if (ReadIniInt(ini, &lg, "Gameplay", "SniperBulletLife", &i)) {
		WriteMemory<uint32_t>(0x558516 + 1, i, true); //1000
	}


	// -- Densities
	if (!G_NoDensities) {

		if (ReadIniFloat(ini, &lg, "Densities", "MapLODdistance", &f)) {
			WriteMemory<float>(0x5F72A4, f, false);
			WriteMemory<float>(0x5F72B8, f + 30.0f, false);
		}

		if (ReadIniFloat(ini, &lg, "Densities", "VehLodDist", &f)) {
			G_VehLodDist = f;
			WriteMemory<float*>(0x528CB3 + 2, &G_VehLodDist, true);
		}

		if (ReadIniFloat(ini, &lg, "Densities", "VehDrawDist", &f)) {
			G_VehDrawDist = f;
			WriteMemory<float*>(0x528D03 + 2, &G_VehDrawDist, true);
		}

		if (ReadIniFloat(ini, &lg, "Densities", "CullDistComps", &f)) {
			G_CullDistComps = f;
			WriteMemory<float*>(0x528C99 + 2, &G_CullDistComps, true);
		}

		if (ReadIniFloat(ini, &lg, "Densities", "VehOccupDrawDist", &f)) {
			WriteMemory<float>(0x5F8618, f, true);
		}

		if (ReadIniFloat(ini, &lg, "Densities", "PedDrawDist", &f)) {  //25.0
			G_PedDrawDist = f;
			WriteMemory<float*>(0x528D1F + 2, &G_PedDrawDist, true);
		}

		if (ReadIniFloat(ini, &lg, "Densities", "RoadblockSpawnDist", &f)) { //80.0
			WriteMemory<float>(0x5EE6E4, -f, true);
			WriteMemory<float>(0x5EE6E8, f, true);
		}

		if (ReadIniFloat(ini, &lg, "Densities", "PedDensity", &f))
		{
			WriteMemory<float>(0x6182F4, f);
		}

		if (ReadIniFloat(ini, &lg, "Densities", "VehDensity", &f))
		{
			WriteMemory<float>(0x6182F8, f);
		}

		if (ReadIniFloat(ini, &lg, "Densities", "VehDespawnOnScr", &f)) { //130.0
			G_VehDespawnOnScr = f;
			WriteMemory<float*>(0x418571+2, &G_VehDespawnOnScr, true);
		}

		if (ReadIniFloat(ini, &lg, "Densities", "VehDespawnOffScr", &f)) { //50.0
			G_VehDespawnOffScr = f;
			WriteMemory<float*>(0x418580+2, &G_VehDespawnOffScr, true);
		}

		if (ReadIniFloat(ini, &lg, "Densities", "PedSpawnDespMult", &f)) {
			WriteMemory<float>(0x5FA8A0, f, true);
			WriteMemory<float>(0x5FA94C, f * 1.5f, true);
		}

		if (ReadIniFloat(ini, &lg, "Densities", "ShadDist_Vehicles", &f)) {
			WriteMemory<float>(0x5FED44, f * f, false);
		}

		if (ReadIniFloat(ini, &lg, "Densities", "ShadDist_Peds", &f)) {
			WriteMemory<float>(0x5FED74, f * f, false);
		}

		/*if (ReadIniFloat(ini, &lg, "Densities", "VehLightDistMult", &f)) {
			WriteMemory<float>(0x5FED60, f * f, false);
			WriteMemory<float>(0x5FED64, f * 0.75f, false);
			WriteMemory<float>(0x5FED68, f * 0.666666667f, false);
			WriteMemory<float>(0x5FED6C, f * 0.004111112f, false);
			WriteMemory<float>(0x5FED5C, f * 0.166666667f, false);
			WriteMemory<float>(0x5FED70, f * 0.222222223f, false);
		}*/

		if (ReadIniInt(ini, &lg, "Densities", "VehLimit", &i)) { //30
			WriteMemory<uint8_t>(0x416597, 0x73, true); // change condition to unsigned (0-255)
			WriteMemory<uint8_t>(0x416596, i, true);
			//WriteMemory<uint8_t>(0x484D19, 0x83, true); // change condition to unsigned (0-255)
			//WriteMemory<uint8_t>(0x484D17, i, true);
		}

		if (ReadIniInt(ini, &lg, "Densities", "PedLimit", &i)) { //25
			WriteMemory<uint32_t>(0x5FA574, i, false);
		}

		if (ReadIniFloat(ini, &lg, "Densities", "ShadDist_AllPerm", &f)) {
			WriteMemory<float>(0x5FED80, f, false);
		}

		if (ReadIniFloat(ini, &lg, "Densities", "TrafLightCoronaDist", &f)) {
			WriteMemory<float>(0x5F00BC, f, false);
		}

		if (ReadIniInt(ini, &lg, "Densities", "FindPathNodeMaxTries", &i)) {
			WriteMemory<uint32_t>(0x42C1B5 + 4, i, false);
		}
	}


	// -- Skid marks

	if (ReadIniFloat(ini, &lg, "Skid Marks", "SkidHeight", &f)) {
		WriteMemory<float>(0x5FEEA0, f, true);
	}

	// -- World
	if (ReadIniBool(ini, &lg, "World", "LockHour")) {
		MakeNOP(0x48C8EB, 5, true);
	}

	if (ReadIniFloat(ini, &lg, "World", "Gravity", &f)) {
		WriteMemory<float>(0x5F68D4, f, false);
	}

	if (ReadIniInt(ini, &lg, "World", "FreezeWeather", &i)) {
		G_FreezeWeather = i;
	}
	else {
		if (G_FreezeWeather >= 0) {
			CWeather::ReleaseWeather();
		}
		G_FreezeWeather = -1;
	}


	// -- Post

	if (bReadOldINI) {

		if (numOldCfgNotFound > 0)
		{
			if (lang == languages::PT)
				lg << "\nAviso: " << numOldCfgNotFound << " configurações não foram encontradas no .ini antigo. Verifique acima.\n";
			else
				lg << "\nWarning: " << numOldCfgNotFound << " configurations has not found on old ini. Check it above.\n";
		}

		bErrorRename = false;

		try {
			filesystem::rename(PLUGIN_PATH("MixSets old.ini"), PLUGIN_PATH("MixSets backup.ini"));
		}
		catch (std::filesystem::filesystem_error& e) {
			if (lang == languages::PT)
			{
				lg << "\nERRO: Não foi possível renomear o arquivo 'MixSets old.ini'. Provavelmente você está com o jogo instalado na pasta Arquivos de Programas ou o arquivo está em uso.\n";
				lg << "Mova seu jogo para outra pasta para o melhor funcionamento deste e outros mods. Ou verifique o arquivo, tente de novo, renomei-o ou delete-o manualmente.\n";
			}
			else {
				lg << "\nERROR: Unable to rename 'MixSets old.ini' file. You probably have the game installed in the Program Files folder or the file is in use.\n";
				lg << "Move your game to another folder for the best working of this and other mods. Or check the file, try again, renamed it, or delete it manually.\n";
			}
			lg << "Error message: " << e.what() << "\n";
			bErrorRename = true;
		}

	}

	lg.flush();
}