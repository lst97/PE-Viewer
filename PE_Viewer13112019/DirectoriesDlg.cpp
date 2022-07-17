#include "PeEditorDlg.h"
#include "windows.h"
#include "resource.h"
// Declar
bool showDirectories(HWND hwndDirectoriesDlg);
void ImportDirectory(HINSTANCE hImageBase, HWND hwndDirectoriesDlg);
void BaseRelocationDirectory(HINSTANCE hImageBase, HWND hwndDirectoriesDlg);
void ExportDirectory(HINSTANCE hImageBase, HWND hwndDirectoriesDlg);

// Grobal
extern TargetFile* g_pcTargetFile;

// Code
BOOL CALLBACK CallBack_DirectoriesDlg(
	HWND hwndDirectoriesDlg,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
) {
	switch (uMsg) {
	case WM_INITDIALOG:
		g_pcTargetFile->setParent(hwndDirectoriesDlg);
		showDirectories(hwndDirectoriesDlg);
		return false;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_BUTTON_DIRECTORIES_CLOSE:
			g_pcTargetFile->pDebugConsol->PrintDebugString("IDC_BUTTON_DIRECTORIES_CLOSE (Close)\n");
			EndDialog(hwndDirectoriesDlg, 0);
			return false;

		case IDC_BUTTON_MORE_IMPORTDIRECTORY:
			g_pcTargetFile->pDebugConsol->PrintDebugString("IDC_BUTTON_MORE_IMPORTDIRECTORY (...)\n");
			ImportDirectory(g_pcTargetFile->getImageBase(), hwndDirectoriesDlg);
			return false;

		case IDC_BUTTON_MORE_BASERELOCATIONDIRECTORY:
			g_pcTargetFile->pDebugConsol->PrintDebugString("IDC_BUTTON_MORE_BASERELOCATIONDIRECTORY (...)\n");
			BaseRelocationDirectory(g_pcTargetFile->getImageBase(), hwndDirectoriesDlg);
			return false;

		case IDC_BUTTON_MORE_EXPORTDIRECTORY:
			g_pcTargetFile->pDebugConsol->PrintDebugString("IDC_BUTTON_MORE_EXPORTDIRECTORY (...)\n");
			ExportDirectory(g_pcTargetFile->getImageBase(), hwndDirectoriesDlg);
			return false;

		default:
			return false;
		}

	default:
		return false;
	}
}

void Directories(HINSTANCE hImageBase, HWND hwndPeEditorDlg) {
	DialogBox(hImageBase, MAKEINTRESOURCE(IDD_DIALOG_DIRECTORIES), hwndPeEditorDlg, CallBack_DirectoriesDlg);
}

bool showDirectories(HWND hwndDirectoriesDlg) {
	unsigned char* pDataDirectories_Temp = (unsigned char*)g_pcTargetFile->pcDataDirectories;
	int L = 0;
	int editControlOffset = 0;
	unsigned char szHex[11] = { "0x" };
	HWND hEditControl = NULL;

	do {
		ASCIItoHex((unsigned char*)pDataDirectories_Temp, szHex + 2, 4);
		hEditControl = GetDlgItem(g_pcTargetFile->getCallerHwnd(), IDC_EDIT_EXPORTDIRECTORY + editControlOffset++);
		SetWindowText(hEditControl, (char*)szHex);
		memset(szHex + 2, 0, 9);
		pDataDirectories_Temp += 4;

		// Show Size
		ASCIItoHex((unsigned char*)pDataDirectories_Temp, szHex + 2, 4);
		hEditControl = GetDlgItem(g_pcTargetFile->getCallerHwnd(), IDC_EDIT_EXPORTDIRECTORY + editControlOffset + 14);
		SetWindowText(hEditControl, (char*)szHex);
		memset(szHex + 2, 0, 9);
		pDataDirectories_Temp += 4;
		L++;
	} while (L != 15);
	fseek(g_pcTargetFile->getFilePtr(), 0, SEEK_SET);

	return true;
}