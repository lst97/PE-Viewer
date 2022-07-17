#include "PeEditorDlg.h"

// Declar
void FileHeader(HINSTANCE hImageBase, HWND hwndPeEditorDlg);
void OptionalHeader(HINSTANCE hImageBase, HWND hwndPeEditorDlg);
void SectionsHeader(HINSTANCE hImageBase, HWND hwndPeEditorDlg);
void Directories(HINSTANCE hImageBase, HWND hwndPeEditorDlg);
class Tools;

// Grobal
TargetFile* g_pcTargetFile = NULL;

// Code
BOOL CALLBACK CallBack_PeEditDlg(
	HWND hwndPeEditDlg,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
) {
	switch (uMsg) {
	case WM_INITDIALOG: 
		g_pcTargetFile->setParent(hwndPeEditDlg);
		if (!g_pcTargetFile->initFile()) 
			EndDialog(hwndPeEditDlg, -1);
		g_pcTargetFile->showDosHeaderInfo();
		return false;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_BUTTON_CLOSE_PEEDITOR:
			g_pcTargetFile->pDebugConsol->PrintDebugString("IDC_BUTTON_CLOSE_PEEDITOR (Close)\n");
			EndDialog(hwndPeEditDlg, 0);
			return false;

		case IDC_BUTTON_FILEHEADER: {
			g_pcTargetFile->setParent(hwndPeEditDlg);
			HINSTANCE hImageBase = g_pcTargetFile->getImageBase();
			g_pcTargetFile->pDebugConsol->PrintDebugString("IDC_BUTTON_FILEHEADER (File Header)\n");
			FileHeader(hImageBase, hwndPeEditDlg);
			return false;
		}

		case IDC_BUTTON_OPTHEADER: {
			g_pcTargetFile->setParent(hwndPeEditDlg);
			HINSTANCE hImageBase = g_pcTargetFile->getImageBase();
			g_pcTargetFile->pDebugConsol->PrintDebugString("IDC_BUTTON_OPTHEADER (Opt Header)\n");
			OptionalHeader(hImageBase, hwndPeEditDlg);
			return false;
		}

		case IDC_BUTTON_SECTION: {
			g_pcTargetFile->setParent(hwndPeEditDlg);
			HINSTANCE hImageBase = g_pcTargetFile->getImageBase();
			g_pcTargetFile->pDebugConsol->PrintDebugString("IDC_BUTTON_SECTIONS (Sections)\n");
			SectionsHeader(hImageBase, hwndPeEditDlg);
			return false;
		}

		case IDC_BUTTON_DIRECTORIES: {
			g_pcTargetFile->setParent(hwndPeEditDlg);
			HINSTANCE hImageBase = g_pcTargetFile->getImageBase();
			g_pcTargetFile->pDebugConsol->PrintDebugString("IDC_BUTTON_DIRECTORIES (Directories)\n");
			Directories(hImageBase, hwndPeEditDlg);
			return false;
		}
		default:
			return false;
		}

	default:
		return false;
	}
}

void PeEditor(HINSTANCE hImageBase, HWND hwndMainDlg, char* pFileLocation, DebugConsol* pDebugConsol) {
	if (!g_pcTargetFile) {
		g_pcTargetFile = new TargetFile(hImageBase, pFileLocation, pDebugConsol);
	}
	DialogBox(hImageBase, MAKEINTRESOURCE(IDD_DIALOG_PEEDIT), hwndMainDlg, CallBack_PeEditDlg);
}

bool TargetFile::createImageBuffer() {
	Tools* pcTools = new Tools(g_pcTargetFile);
	if (!pcTools) {
		pDebugConsol->PrintDebugString("Fail to allocate memory for IMAGE_DATA_DIRECTORY[16]. (0x%08X)\n", GetLastError());
		return false;
	}
	pcTools->createRawImageBuffer(fpOrignal, pImageBuffer);

	return true;
}

bool TargetFile::saveImportDescriptorInfo(OUT IMAGE_IMPORT_DESCRIPTOR* pcImportDescriptor, unsigned int numberOfImportDescriptor) {

	unsigned char* numberOfImportDescriptor_Temp = (unsigned char*)pcImportDescriptor;
	PIMAGEBUFFER pImageBuffer_Temp = pImageBuffer;
	pImageBuffer_Temp += pcDosHeader->e_lfanew + 0x18 + 0x60 + 0x08;
	unsigned int descriptorRVA = *((unsigned int*)pImageBuffer_Temp + 0);
	unsigned int descriptorFOA = 0;
	Tools *pcTools = new Tools(g_pcTargetFile);
	unsigned int descriptorOffset = pcTools->RvaToFoa(descriptorRVA, &descriptorFOA, g_pcTargetFile);

	pImageBuffer_Temp = pImageBuffer;
	pImageBuffer_Temp += descriptorFOA;
	int L = numberOfImportDescriptor * 0x14;

	// can use memcpy();
	do {
		*numberOfImportDescriptor_Temp = *pImageBuffer_Temp;
		pImageBuffer_Temp++;
		numberOfImportDescriptor_Temp++;
		L--;
	} while (L != 0);

	free(pcTools);
	return true;
}

bool TargetFile::saveExportDirectoriesInfo(OUT IMAGE_EXPORT_DIRECTORY* pcExportDirectory) {

	Tools* pcTools = new Tools(g_pcTargetFile);
	unsigned int exportDirectoryFOA = 0;
	pcTools->RvaToFoa(g_pcTargetFile->pcDataDirectories->VirtualAddress, &exportDirectoryFOA, g_pcTargetFile);
	IMAGE_EXPORT_DIRECTORY* pcExportDirectory_Temp = pcExportDirectory;
	PIMAGEBUFFER pImageBuffer_ExportDirectory = pImageBuffer + exportDirectoryFOA;

	memcpy(pcExportDirectory_Temp, pImageBuffer_ExportDirectory, 0x28);

	free(pcTools);
	return true;
}
