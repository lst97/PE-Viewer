#pragma once
#ifndef PEEDITORDLG_H_
#define PEEDITORDLG_H_

#define IN
#define OUT

#include "DebugConsol.h"
#include "PeEditorDlg.h"
#include "FileHeaderDlg.h"
#include "OptionalHeaderDlg.h"
#include "ASCIItoHex.h"
#include "Tools.h"

// Declar
typedef unsigned char* PIMAGEBUFFER;
typedef unsigned char IMAGEBUFFER;

bool ASCIItoHex(IN unsigned char* szString, OUT unsigned char* szBuffer, int size);

// Grobal
class TargetFile {
	
public:
	DebugConsol* pDebugConsol;

	IMAGE_DOS_HEADER* pcDosHeader;
	IMAGE_FILE_HEADER* pcFileHeader;
	IMAGE_OPTIONAL_HEADER* pcOptHeader;
	IMAGE_SECTION_HEADER* pcSectionHeader;
	IMAGE_DATA_DIRECTORY* pcDataDirectories;

	IMAGE_IMPORT_DESCRIPTOR* pcImportDescriptor;
	IMAGE_EXPORT_DIRECTORY* pcExportDirectory;

	// I have not yet create those class for storage the data from ImageBuffer.
	//IMAGE_THUNK_DATA* pcImageThinkData;
	//IMAGE_BASE_RELOCATION* pcBaseRelocation;


private:
	HINSTANCE hImageBase;
	HWND hwnd;
	FILE* fpOrignal;
	PIMAGEBUFFER pImageBuffer;
	char* pFileLocation;
	unsigned int fileSize;
	unsigned int imageSize;
	unsigned int numberOfImportDescriptor;

private:
	//bool saveDosHeaderInfo(OUT IMAGE_DOS_HEADER* pSaveDosHeaderInfo, FILE* fpOrignal);
	//bool saveFileHeaderInfo(OUT IMAGE_FILE_HEADER* pSaveFileHeaderInfo, FILE* fpOrignal);
	//bool openFile(char* pFileLocation);
	//bool isValid();
	bool createImageBuffer();
	bool openFile(char* pFileLocation) {
		fpOrignal = fopen(pFileLocation, "rb");
		if (!fpOrignal)
			return false;

		return true;
	}

	bool isValid() {
		// Check MZ
		char szMzBuffer[3] = { 0 };
		char* pszMzBuffer = szMzBuffer;
		unsigned int L = 2;

		while (L != 0) {
			*pszMzBuffer = fgetc(fpOrignal);
			pszMzBuffer++;
			L--;
		}
		szMzBuffer[2] = 0;
		pszMzBuffer = szMzBuffer;
		fseek(fpOrignal, 0, SEEK_SET);

		if (strcmp(szMzBuffer, "MZ")) { 	// Not MZ
			pDebugConsol->PrintDebugString("This file is not a valid protable executable file. (Not MZ) [ FAIL ]\n", 0);
			return false;
		}
		pDebugConsol->PrintDebugString("Is Valid DOS Header. [  OK  ]\n");

		// Check PE (Signature)
		unsigned int e_lfanew = 0;
		unsigned char e_lfanew_Offset = 0x3C;

		fseek(fpOrignal, e_lfanew_Offset, SEEK_CUR);
		e_lfanew |= fgetc(fpOrignal);
		fseek(fpOrignal, 0, SEEK_SET);

		char szSignatureBuffer[3];
		char* pszSignatureBuffer = szSignatureBuffer;
		fseek(fpOrignal, e_lfanew, SEEK_CUR);
		L = 2;
		do {
			*pszSignatureBuffer = fgetc(fpOrignal);
			pszSignatureBuffer++;
			L--;
		} while (L != 0);
		*pszSignatureBuffer = fgetc(fpOrignal);
		fseek(fpOrignal, 0, SEEK_SET);

		if (strcmp(szSignatureBuffer, "PE")) {
			pDebugConsol->PrintDebugString("This file is not a valid protable executable file. (Not PE) [ FAIL ]\n", 0);
			return false;
		}
		pDebugConsol->PrintDebugString("Is Valid protable executable file. [  OK  ]\n");

		// Check 32Bit CPU

		unsigned char szMachineBuffer[3] = { 0 };
		unsigned char* pszMachineBuffer = szMachineBuffer;
		fseek(fpOrignal, e_lfanew + 4, SEEK_CUR);

		L = 2;
		do {
			*pszMachineBuffer = fgetc(fpOrignal);
			pszMachineBuffer++;
			L--;
		} while (L != 0);
		*pszMachineBuffer = fgetc(fpOrignal);
		fseek(fpOrignal, 0, SEEK_SET);
		
		if ((unsigned short)*((unsigned short*)szMachineBuffer + 0) != 0x14C) {
			pDebugConsol->PrintDebugString("This file can not be running on 32Bit CPU. [ FAIL ]\n");
			return false;
		}
		pDebugConsol->PrintDebugString("Is Valid for 32Bit CPU. [  OK  ]\n");

		// Check 32Bit executable
		unsigned char szMagicBuffer[3] = { 0 };
		unsigned char* pszMagicBuffer = szMagicBuffer;
		fseek(fpOrignal, e_lfanew + 0x18, SEEK_CUR);

		L = 2;
		do {
			*pszMagicBuffer = fgetc(fpOrignal);
			pszMagicBuffer++;
			L--;
		} while (L != 0);
		*pszMagicBuffer = fgetc(fpOrignal);
		fseek(fpOrignal, 0, SEEK_SET);

		if ((unsigned short) * ((unsigned short*)szMagicBuffer + 0) != 0x10B) {
			pDebugConsol->PrintDebugString("NOT a valid 32Bit executable file. [ FAIL ]\n");
			return false;
		}
		pDebugConsol->PrintDebugString("Is Valid 32Bit executable. [  OK  ]\n");

		return true;
	}

	bool saveDosHeaderInfo(OUT IMAGE_DOS_HEADER* pcDosHeaderInfo, FILE* fpOrignal) {

		int L = 0x40;
		char* pDosHeaderInfo_Temp = (char*)pcDosHeaderInfo;

		do {
			*pDosHeaderInfo_Temp = fgetc(fpOrignal);
			pDosHeaderInfo_Temp++;
			L--;
		} while (L != 0);
		*pDosHeaderInfo_Temp = fgetc(fpOrignal);
		fseek(fpOrignal, 0, SEEK_SET);

		return true;
	}

	bool saveFileHeaderInfo(OUT IMAGE_FILE_HEADER* pcFileHeaderInfo, FILE* fpOrignal) {

		int L = 0x14;
		char* pFileHeaderInfo_Temp = (char*)pcFileHeaderInfo;
		fseek(fpOrignal, pcDosHeader->e_lfanew + 4, SEEK_CUR);

		do {
			*pFileHeaderInfo_Temp = fgetc(fpOrignal);
			pFileHeaderInfo_Temp++;
			L--;
		} while (L != 0);
		*pFileHeaderInfo_Temp = fgetc(fpOrignal);
		fseek(fpOrignal, 0, SEEK_SET);

		return true;
	}

	bool saveOptHeaderInfo(OUT IMAGE_OPTIONAL_HEADER* pcOptHeaderInfo, FILE* fpOrignal) {

		int L = pcFileHeader->SizeOfOptionalHeader;
		char* pOptHeaderInfo_Temp = (char*)pcOptHeaderInfo;
		fseek(fpOrignal, pcDosHeader->e_lfanew + 0x18, SEEK_CUR);

		do {
			*pOptHeaderInfo_Temp = fgetc(fpOrignal);
			pOptHeaderInfo_Temp++;
			L--;
		} while (L != 0);
		*pOptHeaderInfo_Temp = fgetc(fpOrignal);
		fseek(fpOrignal, 0, SEEK_SET);

		return true;
	}

	bool saveSectionHeaderInfo(OUT IMAGE_SECTION_HEADER* pcSectionHeaderInfo, FILE* fpOrignal) {

		int L = 0x28 * pcFileHeader->NumberOfSections;
		char* pSectionHeaderInfo_Temp = (char*)pcSectionHeaderInfo;
		fseek(fpOrignal, pcDosHeader->e_lfanew + 0x18 + pcFileHeader->SizeOfOptionalHeader, SEEK_CUR);

		do {
			*pSectionHeaderInfo_Temp = fgetc(fpOrignal);
			pSectionHeaderInfo_Temp++;
			L--;
		} while (L != 0);
		*pSectionHeaderInfo_Temp = fgetc(fpOrignal);
		fseek(fpOrignal, 0, SEEK_SET);

		return true;
	}

	bool saveDirectoriesInfo(OUT IMAGE_DATA_DIRECTORY* pcDataDirectories) {

		int L = 0x80;
		unsigned char* pDataDirectories_Temp = (unsigned char*)pcDataDirectories;
		PIMAGEBUFFER pImageBuffer_Temp = pImageBuffer;
		pImageBuffer_Temp += pcDosHeader->e_lfanew + 0x18 + 0x60;

		do {
			*pDataDirectories_Temp = *pImageBuffer_Temp;
			pImageBuffer_Temp++;
			pDataDirectories_Temp++;
			L--;
		} while (L != 0);
		*pDataDirectories_Temp = *pImageBuffer_Temp;

		return true;
	}
	bool saveExportDirectoriesInfo(OUT IMAGE_EXPORT_DIRECTORY* pcDataDirectories);
	bool saveImportDescriptorInfo(OUT IMAGE_IMPORT_DESCRIPTOR* pcImportDescriptor, unsigned int numberOfImportDescriptor);

public:
	//void setParent(HWND hwnd);
	//void getImageBase(OUT HINSTANCE* hImageBase);
	//bool initFile();
	//bool showDosHeaderInfo(IMAGE_DOS_HEADER* pcDosHeader);
	//bool showFileHeaderInfo(IMAGE_FILE_HEADER* pcFileHeader);
	//TargetFile(HINSTANCE hImageBase, char* pFileLocation, DebugConsol* pDebugConsol);
	//~TargetFile();

	TargetFile(HINSTANCE hImageBase, char* pFileLocation, DebugConsol* pDebugConsol) {
		this->hImageBase = hImageBase;
		this->pDebugConsol = pDebugConsol;
		this->pFileLocation = pFileLocation;

		fpOrignal = NULL;
		pImageBuffer = NULL;

		pcDosHeader = NULL;
		pcFileHeader = NULL;
		pcOptHeader = NULL;
		pcSectionHeader = NULL;

	}
	~TargetFile() {

	}

	TargetFile* getTargetFilePtr() {
		return this;
	}

	HINSTANCE getImageBase() {
		return this->hImageBase;
	}

	HWND getCallerHwnd() {
		return (this->hwnd);
	}

	FILE* getFilePtr() {
		return this->fpOrignal;
	}

	PIMAGEBUFFER getImageBuffer() {
		return this->pImageBuffer;
	}

	unsigned int getNumberOfImportDescriptor() {
		return this->numberOfImportDescriptor;
	}

	void setParent(HWND hwnd) {
		this->hwnd = hwnd;
	}

	void setFileSize(unsigned int fileSize) {
		this->fileSize = fileSize;
	}

	void setImageSize(unsigned int imageSize) {
		this->imageSize = imageSize;
	}

	void setImageBufferPtr(PIMAGEBUFFER pImageBuffer) {
		this->pImageBuffer = pImageBuffer;
	}

	void setNumberOfImportDescriptor(unsigned int numberOfImportDescriptor) {
		this->numberOfImportDescriptor = numberOfImportDescriptor;
	}

	bool initFile() {
		if (!openFile(pFileLocation)) {
			pDebugConsol->PrintDebugString("ERROR! Fail to open the specific file! (0x%08X)\n", GetLastError());
			MessageBox(hwnd, "File can not be open!", "Error", MB_OK | MB_ICONERROR);
			return false;
		}

		if (!isValid()) {
			MessageBox(hwnd, "This is not a valid EXE!", "Error", MB_OK | MB_ICONERROR);
			return false;
		}

		IMAGE_DOS_HEADER* pcDosHeader = new IMAGE_DOS_HEADER();
		if (!pcDosHeader) {
			pDebugConsol->PrintDebugString("Fail to allocate memory for IMAGE_DOS_HEADER. (0x%08X)\n", GetLastError());
			return false;
		}
		saveDosHeaderInfo(pcDosHeader, fpOrignal);
		this->pcDosHeader = pcDosHeader;
		pDebugConsol->PrintDebugString("saveDosHeaderInfo()\n");

		IMAGE_FILE_HEADER* pcFileHeader = new IMAGE_FILE_HEADER();
		if (!pcFileHeader) {
			pDebugConsol->PrintDebugString("Fail to allocate memory for IMAGE_FILE_HEADER. (0x%08X)\n", GetLastError());
			return false;
		}
		saveFileHeaderInfo(pcFileHeader, fpOrignal);
		this->pcFileHeader = pcFileHeader;
		pDebugConsol->PrintDebugString("saveFileHeaderInfo()\n");

		IMAGE_OPTIONAL_HEADER* pcOptHeader = new IMAGE_OPTIONAL_HEADER();
		if (!pcOptHeader) {
			pDebugConsol->PrintDebugString("Fail to allocate memory for IMAGE_OPTIONAL_HEADER. (0x%08X)\n", GetLastError());
			return false;
		}
		saveOptHeaderInfo(pcOptHeader, fpOrignal);
		this->pcOptHeader = pcOptHeader;
		pDebugConsol->PrintDebugString("saveOptHeaderInfo()\n");

		IMAGE_SECTION_HEADER* pcSectionHeader = new IMAGE_SECTION_HEADER[pcFileHeader->NumberOfSections];
		memset(pcSectionHeader, 0, 0x28 * pcFileHeader->NumberOfSections);
		if (!pcSectionHeader) {
			pDebugConsol->PrintDebugString("Fail to allocate memory for IMAGE_SECTION_HEADER. (0x%08X)\n", GetLastError());
			return false;
		}
		saveSectionHeaderInfo(pcSectionHeader, fpOrignal);
		this->pcSectionHeader = pcSectionHeader;
		pDebugConsol->PrintDebugString("saveSectionHeaderInfo()\n");

		// Create RAW Image buffer as the EXE is valid
		pDebugConsol->PrintDebugString("createImageBuffer()... ", GetLastError());
		createImageBuffer();

		if (!createImageBuffer()) 
#ifdef DEBUGMODE
			return false;
		printf("[  OK  ]\n");
#endif // !DEBUGMODE

		IMAGE_DATA_DIRECTORY* pcDataDirectories = new IMAGE_DATA_DIRECTORY[16];
		memset(pcDataDirectories, 0, 0x80);
		if (!pcDataDirectories) {
			pDebugConsol->PrintDebugString("Fail to allocate memory for IMAGE_DATA_DIRECTORY[16]. (0x%08X)\n", GetLastError());
			return false;
		}
		saveDirectoriesInfo(pcDataDirectories);
		this->pcDataDirectories = pcDataDirectories;
		pDebugConsol->PrintDebugString("saveDirectoriesInfo()\n");

		PIMAGEBUFFER pImageBuffer_Temp = pImageBuffer;
		pcExportDirectory = new IMAGE_EXPORT_DIRECTORY;
		saveExportDirectoriesInfo(pcExportDirectory);
		pDebugConsol->PrintDebugString("saveExportDirectoriesInfo()\n");

		pImageBuffer_Temp = pImageBuffer;
		pImageBuffer_Temp += (pcDosHeader->e_lfanew + 0x18 + 0x60 + 0x08 + 0x4);
		unsigned int numberOfImportDescriptor = (*((unsigned int*)pImageBuffer_Temp + 0) / 0x14) - 1;
		IMAGE_IMPORT_DESCRIPTOR* pcImportDescriptor = new IMAGE_IMPORT_DESCRIPTOR[numberOfImportDescriptor];
		memset(pcImportDescriptor, 0, numberOfImportDescriptor);
		if (!pcImportDescriptor) {
			pDebugConsol->PrintDebugString("Fail to allocate memory for IMAGE_DATA_DIRECTORY[16]. (0x%08X)\n", GetLastError());
			return false;
		}
		setNumberOfImportDescriptor(numberOfImportDescriptor);
		saveImportDescriptorInfo(pcImportDescriptor, numberOfImportDescriptor);
		this->pcImportDescriptor = pcImportDescriptor;
		pDebugConsol->PrintDebugString("saveImportDescriptorInfo()\n");

		return true;
	}

	bool showDosHeaderInfo() {
		unsigned short* pcDosHeader_Temp = (unsigned short*)this->pcDosHeader;
		int L = 0x40;
		int i = 2;
		int editControlOffset = 0;
		unsigned char szHex[11] = { "0x" };

		do {
			if (i == 2) {
				ASCIItoHex((unsigned char*)pcDosHeader_Temp, szHex + 2, 2);
				HWND hEditControl = GetDlgItem(hwnd, IDC_EDIT_MAGICNUMBER + editControlOffset);
				SetWindowText(hEditControl, (char*)szHex);
				memset(szHex + 2, 0, 9);
				i = 0;
				editControlOffset++;
				pcDosHeader_Temp++;
			}
			L -= 2;
			i++;
		} while (L != 0);
		ASCIItoHex((unsigned char*) & (pcDosHeader->e_lfanew), szHex + 2, 4);
		HWND hEditControl = GetDlgItem(hwnd, IDC_EDIT_MAGICNUMBER + editControlOffset);
		SetWindowText(hEditControl, (char*)szHex);
		fseek(fpOrignal, 0, SEEK_SET);

		return true;
	}

	bool showFileHeaderInfo() {
		unsigned char* pcFileHeader_Temp = (unsigned char*)this->pcFileHeader;
		char fileHeaderSize[7] = { 2, 2, 4, 4, 4, 2, 2 };

		int L = 0;
		int editControlOffset = 0;
		unsigned char szHex[11] = { "0x" };

		do {
			ASCIItoHex((unsigned char*)pcFileHeader_Temp, szHex + 2, fileHeaderSize[L]);
			HWND hEditControl = GetDlgItem(hwnd, IDC_EDIT_MACHINE + editControlOffset);
			SetWindowText(hEditControl, (char*)szHex);
			memset(szHex + 2, 0, 9);
			editControlOffset++;
			pcFileHeader_Temp += fileHeaderSize[L];
			L++;
		} while (L != 7);
		fseek(fpOrignal, 0, SEEK_SET);

		return true;
	}

	bool showOptHeaderInfo() {
		unsigned char* pOptHeader_Temp = (unsigned char*)this->pcOptHeader;
		char fileHeaderSize[30] = { 2, 1, 1, 4, 4, 4, 4, 4, 4, 4,
									4, 4, 2, 2, 2, 2, 2, 2, 4, 4, 
									4, 4, 2, 2, 4, 4, 4, 4, 4, 4};

		int L = 0;
		int editControlOffset = 0;
		unsigned char szHex[11] = { "0x" };

		do {
			ASCIItoHex((unsigned char*)pOptHeader_Temp, szHex + 2, fileHeaderSize[L]);
			HWND hEditControl = GetDlgItem(hwnd, IDC_EDIT_MAGIC + editControlOffset++);
			SetWindowText(hEditControl, (char*)szHex);
			memset(szHex + 2, 0, 9);
			pOptHeader_Temp += fileHeaderSize[L];
			L++;
		} while (L != 30);
		fseek(fpOrignal, 0, SEEK_SET);
		return true;
	}

	bool showDirectriesInfo() {
		unsigned char* pcDataDirectories_Temp = (unsigned char*)this->pcDataDirectories;

		int L = 0;
		int editControlOffset = 0;
		unsigned char szHex[11] = { "0x" };

		do {
			ASCIItoHex((unsigned char*)pcDataDirectories_Temp, szHex + 2, 4);
			HWND hEditControl = GetDlgItem(hwnd, IDC_EDIT_EXCEPTIONDIRECTORY + editControlOffset++);
			SetWindowText(hEditControl, (char*)szHex);
			memset(szHex + 2, 0, 9);
			pcDataDirectories_Temp += 4;
			L++;
		} while (L != 16);
		fseek(fpOrignal, 0, SEEK_SET);
		return true;
	}
};
#endif