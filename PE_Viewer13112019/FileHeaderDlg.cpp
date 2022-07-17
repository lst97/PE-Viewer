#include "FileHeaderDlg.h"

// Declar

// Grobal
extern TargetFile* g_pcTargetFile;

BOOL CALLBACK CallBack_FileHeaderDlg(
	HWND hwndFileHeaderDlg,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
) {
	switch (uMsg) {
	case WM_INITDIALOG:
		g_pcTargetFile->setParent(hwndFileHeaderDlg);
		g_pcTargetFile->showFileHeaderInfo();
		return true;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_BUTTON_FILEHEADER_CANCEL:
			g_pcTargetFile->pDebugConsol->PrintDebugString("IDC_BUTTON_FILEHEADER_CANCEL (Cancel)\n");
			EndDialog(hwndFileHeaderDlg, 0);
			return true;

		default:
			return false;
		}

	default:
		return false;
	}
}

void FileHeader(HINSTANCE hImageBase, HWND hwndPeEditorDlg) {
	DialogBox(hImageBase, MAKEINTRESOURCE(IDD_DIALOG_FILEHEADER), hwndPeEditorDlg, CallBack_FileHeaderDlg);
}
