#include "OptionalHeaderDlg.h"

extern TargetFile* g_pcTargetFile;

BOOL CALLBACK CallBack_OptionalHeaderDlg(
	HWND hwndOptionalHeaderDlg,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
) {
	switch (uMsg) {
	case WM_INITDIALOG:
		g_pcTargetFile->setParent(hwndOptionalHeaderDlg);
		g_pcTargetFile->showOptHeaderInfo();
		return false;

	case WM_COMMAND:

		switch (LOWORD(wParam)) {
		case IDC_BUTTON_OPTHEADER_CANCEL:
			g_pcTargetFile->pDebugConsol->PrintDebugString("IDC_BUTTON_OPTHEADER_CANCEL (Cancel)\n");
			EndDialog(hwndOptionalHeaderDlg, 0);
			return false;

		default:
			return false;
		}

	default:
		return false;
	}
}

void OptionalHeader(HINSTANCE hImageBase, HWND hwndPeEditorDlg) {
	DialogBox(hImageBase, MAKEINTRESOURCE(IDD_DIALOG_OPTHEADER), hwndPeEditorDlg, CallBack_OptionalHeaderDlg);
}
