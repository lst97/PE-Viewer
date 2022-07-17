#include "Header.h"

// Declar
BOOL CALLBACK CallBack_MainDlg(HWND hwndMainDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK CallBack_PeEditDlg(HWND hwndPeEditDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
void PeEditor(HINSTANCE hImageBase, HWND hwndMainDlg, char* pFileLocation, DebugConsol* pDebugConsol);

// Global
HINSTANCE g_hImageBase = 0;
DebugConsol* pDebugConsol = 0;
unsigned int debugStatusFlag = 0;	// init|Load|

// Code
int APIENTRY WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
) {
	// Init common control
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icex);

	g_hImageBase = hInstance;

	FILE* fpDebugConsol = NULL;

#ifdef DEBUGMODE
	pDebugConsol = new DebugConsol(hInstance);
	pDebugConsol->CreateDebugConsol(fpDebugConsol);

	pDebugConsol->PrintDebugString("Starting...\n");

#endif

	DialogBox(g_hImageBase, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, CallBack_MainDlg);

	return 0;
}

BOOL CALLBACK CallBack_MainDlg(
	HWND hwndMainDlg,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
) {

	switch (uMsg) {
	case WM_INITDIALOG:
		pDebugConsol->PrintDebugString("Initialing...\n");

		return true;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_BUTTON_PEEDIT:
			pDebugConsol->PrintDebugString("IDC_BUTTON_PEEDIT (PE Editor)\n");
			PeEditor(g_hImageBase, hwndMainDlg, (char*)".\\temp.exe", pDebugConsol);
			return false;

		case IDC_BUTTON_EXIT:
			PostQuitMessage(0);
			return false;

		default:
			return false;
		}
	case WM_SHOWWINDOW:
		if (!(debugStatusFlag && 0b10)) {
			pDebugConsol->PrintDebugString("SUCCESS!\n");
			debugStatusFlag = 3;
		}

		return false;

	default:
		return false;
	}
}
