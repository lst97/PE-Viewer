#pragma once
#ifndef DEBUGCONSOL_H_
#define DEBUGCONSOL_H_

#include "Header.h"
#define DEBUGMODE

class DebugConsol {
private:
	HINSTANCE g_hImageBase;
	FILE* fpDebugConsol;
	char outputDebugStringBuffer[4096];
	SYSTEMTIME pSystemTime;

public:
	DebugConsol(HINSTANCE g_hImageBase) {
		this->g_hImageBase = g_hImageBase;
		fpDebugConsol = NULL;
		memset(outputDebugStringBuffer, 0, 4096);
	}
	~DebugConsol() {
		TerminateProcess(g_hImageBase, -1);
	}

	bool CreateDebugConsol(OUT FILE* fpDebugConsol) {
		if (!AllocConsole()) {
			GetLastError();
			sprintf(outputDebugStringBuffer, "Fail to allocate consol for main process! (0x%08X)\n", GetLastError());
			OutputDebugString(outputDebugStringBuffer);
			return false;
		}

		this->fpDebugConsol = fpDebugConsol;
		fpDebugConsol = freopen("CONOUT$", "w", stdout);
		return true;
	}

	void PrintTime() {
		GetSystemTime(&pSystemTime);
		printf("[%02d:%02d:%02d:%03d] ", pSystemTime.wHour, pSystemTime.wMinute, pSystemTime.wSecond, pSystemTime.wMilliseconds);
		return;
	}

	void PrintDebugString(const char* pStringBuffer,unsigned int errorCode = 0) {
	#ifdef DEBUGMODE
		PrintTime();
		if (errorCode) {
			printf(pStringBuffer, errorCode);
			return;
		}
		printf(pStringBuffer);
	#endif // DEBUGMODE
	}
};

#endif // DEBUGCONSOL_H_