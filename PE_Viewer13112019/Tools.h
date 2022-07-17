#pragma once
#ifndef TOOLS_H_
#define TOOLS_H_
#include "stdio.h"
#include "windows.h"
#include "PeEditorDlg.h"

typedef unsigned char* PIMAGEBUFFER;
typedef unsigned char IMAGEBUFFER;

class TargetFile;

class Tools {
private:
	TargetFile* pcTargetFile;

	unsigned int getFileSize(FILE* fpOrignal);
	bool isValidRVA(unsigned int RVA, unsigned int numberOfSection, unsigned int sectionAlignment, unsigned int imageSize);

public:
	Tools(TargetFile* pcTargetFile);

	bool createRawImageBuffer(IN FILE* fpOrignal, OUT PIMAGEBUFFER pImageBuffer);
	int RvaToFoa(IN unsigned int RVA, OUT unsigned int* FOA, IN TargetFile* pcTargetFile);
	bool getSectionName(IN unsigned int RVA, OUT char* szBuffer, IN TargetFile* pcTargetFile);
};

#endif // !TOOLS_H_