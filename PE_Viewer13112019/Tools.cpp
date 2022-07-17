#include "Tools.h"
Tools::Tools(TargetFile* pcTargetFile) {
	this->pcTargetFile = pcTargetFile;
}

unsigned int Tools::getFileSize(FILE* fpOrignal) {
	unsigned int fileSize = 0;
	unsigned int pointerToRawData = 0;
	unsigned int sizeOfRawData = 0;

	IMAGE_SECTION_HEADER* pcSectionHeader_Temp = NULL;
	pcSectionHeader_Temp = pcTargetFile->pcSectionHeader;
	pcSectionHeader_Temp += pcTargetFile->pcFileHeader->NumberOfSections - 1;

	sizeOfRawData = pcSectionHeader_Temp->SizeOfRawData;
	pointerToRawData = pcSectionHeader_Temp->PointerToRawData;

	fileSize = pointerToRawData + sizeOfRawData;
	return fileSize;
}

bool Tools::createRawImageBuffer(IN FILE* fpOrignal, OUT PIMAGEBUFFER pImageBuffer) {
	unsigned int fileSize = 0;
	FILE* fpImageBuffer = 0;
	fileSize = getFileSize(fpImageBuffer);
	pcTargetFile->setFileSize(fileSize);

	PIMAGEBUFFER pNewImageBuffer = new IMAGEBUFFER[fileSize];
	if (!pNewImageBuffer) {
		pcTargetFile->pDebugConsol->PrintDebugString("Fail to allocate memory for PIMAGEBUFFER. (0x%08X)\n", GetLastError());
		return false;
	}
	memset(pNewImageBuffer, 0, fileSize);
	pcTargetFile->setImageBufferPtr(pNewImageBuffer);

	// Write data to Image Buffer
	unsigned int L = fileSize;
	do {
		*pNewImageBuffer = fgetc(fpOrignal);
		pNewImageBuffer++;
		L--;
	} while (L != 0);
	*pNewImageBuffer = fgetc(fpOrignal);
	fseek(fpOrignal, 0, SEEK_SET);

	return true;
}

int Tools::RvaToFoa(IN unsigned int RVA, OUT unsigned int* FOA, IN TargetFile* pcTargetFile) {
	unsigned int imageSize = 0;
	unsigned int numberOfSection = pcTargetFile->pcFileHeader->NumberOfSections;
	unsigned int sectionAlignment = pcTargetFile->pcOptHeader->SectionAlignment;
	unsigned int RvaPointingToSection = 0;

	IMAGE_SECTION_HEADER* pcImageSectionHeader_Temp = pcTargetFile->pcSectionHeader;
	pcImageSectionHeader_Temp += numberOfSection - 1;
	imageSize = pcImageSectionHeader_Temp->VirtualAddress + sectionAlignment * (pcImageSectionHeader_Temp->SizeOfRawData % sectionAlignment + 1);
	pcTargetFile->setImageSize(imageSize);
	if (!isValidRVA(RVA, numberOfSection, sectionAlignment, imageSize))
		return -1;

	if (RVA <= pcTargetFile->pcOptHeader->SizeOfHeaders) {
		*FOA = RVA;
		return 0;
	}

	// posistion which section of the RVA pointing to.
	pcImageSectionHeader_Temp = pcTargetFile->pcSectionHeader;
	while (RVA >= pcImageSectionHeader_Temp->VirtualAddress) {
		RvaPointingToSection++;
		pcImageSectionHeader_Temp++;
	}
	RvaPointingToSection--;
	pcImageSectionHeader_Temp--;

	unsigned int addressOffset = RVA - pcImageSectionHeader_Temp->VirtualAddress;
	*FOA = pcImageSectionHeader_Temp->PointerToRawData + addressOffset;

	return 0;
}

bool Tools::isValidRVA(unsigned int RVA, unsigned int numberOfSection, unsigned int sectionAlignment, unsigned int imageSize) {

	IMAGE_SECTION_HEADER* pcImageSectionHeader_Temp = pcTargetFile->pcSectionHeader;

	pcImageSectionHeader_Temp += numberOfSection - 1;

	if (RVA > imageSize || imageSize < 0) {
		pcTargetFile->pDebugConsol->PrintDebugString("Invalid Relative Virtual Address.\n");
		return false;
	}

	return true;
}

bool Tools::getSectionName(IN unsigned int RVA, OUT char* szBuffer, IN TargetFile* pcTargetFile) {

	unsigned int imageSize = 0;
	unsigned int numberOfSection = pcTargetFile->pcFileHeader->NumberOfSections;
	unsigned int sectionAlignment = pcTargetFile->pcOptHeader->SectionAlignment;
	unsigned int RvaPointingToSection = 0;

	IMAGE_SECTION_HEADER* pcImageSectionHeader_Temp = pcTargetFile->pcSectionHeader;
	pcImageSectionHeader_Temp += numberOfSection - 1;
	imageSize = pcImageSectionHeader_Temp->VirtualAddress + sectionAlignment * (pcImageSectionHeader_Temp->SizeOfRawData % sectionAlignment + 1);

	pcImageSectionHeader_Temp = pcTargetFile->pcSectionHeader;
	while (RVA >= pcImageSectionHeader_Temp->VirtualAddress) {
		RvaPointingToSection++;
		pcImageSectionHeader_Temp++;
	}
	RvaPointingToSection--;
	pcImageSectionHeader_Temp--;

	PIMAGEBUFFER pSectionHeader_Temp = (PIMAGEBUFFER)(pcTargetFile->pcSectionHeader + RvaPointingToSection);
	memcpy(szBuffer, pSectionHeader_Temp, 8);

	return true;
}