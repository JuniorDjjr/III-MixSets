#pragma once

#ifndef MIXSETS_H
#define MIXSETS_H

#include "plugin.h"

#include "IniReader/IniReader.h"

using namespace std;

class MixSetsIII
{
public:

	enum class languages
	{
		PT,
		EN
	};

	static inline bool Read;
	static inline fstream lg;

	static inline bool bProcessOnceOnScripts, bProcessOnceAfterIntro, bNoCLEO;
	static inline languages lang;

	static inline float G_RainGroundSplashNum, G_WheelTurnSpeed, G_VehLodDist, G_VehDrawDist, G_CullDistComps, G_PedDrawDist, G_VehDespawnOnScr, G_VehDespawnOffScr;

	static inline int gameVersion;
	static inline bool bEnabled, bReadOldINI, bParsePreserveComments, bErrorRename, bOLA, bIniFailed, bVersionFailed, G_NoDensities, bReloading;

	static inline int G_i, G_FPSlimit, G_ProcessPriority, G_FreezeWeather, G_HowManyMinsInDay;

	static inline float zero;

	static inline int numOldCfgNotFound;

	static inline string G_ReloadCommand;

	static inline char G_NoMoneyZeros_Pos[4];
	static inline char G_NoMoneyZeros_Neg[5];

	////////////////////////////////////////////////

	MixSetsIII();

	static void ReadIni();
	static void ReadIni_BeforeFirstFrame();

	static void ShowModMessages();
	static void ProcessPriority();
	static void SetIdlePriority();
	static inline bool FileExists(const string& name);
	static void ReadOldINI(CIniReader ini, fstream* lg, string section, string key);
	static bool ReadIniFloat(CIniReader ini, fstream* lg, string section, string key, float* f);
	static bool ReadIniInt(CIniReader ini, fstream* lg, string section, string key, int* i);
	static bool ReadIniBool(CIniReader ini, fstream* lg, string section, string key);

	static void IncreaseMemoryValueIfValid(uintptr_t address, int32_t value, uint8_t validation, bool vp);
	static void IncreaseMemoryValueIfValid_Byte(uintptr_t address, int8_t value, uint8_t validation, bool vp);

	static void asm_fmul(float f);

	static const unsigned int GET_SCRIPT_STRUCT_NAMED = 0x10AAA;
};

#endif