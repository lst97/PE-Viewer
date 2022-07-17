#include "SectionsHeaderDlg.h"

// Declar
bool showSectionHeaderInfo(int selectedRow);

// Grobal
extern TargetFile* g_pcTargetFile;
int* g_pSelectedRow = 0;

// Code
BOOL CALLBACK CallBack_SectionEditorDlg(
	HWND hwndSectionEditorDlg,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
) {
	switch (uMsg) {
	case WM_INITDIALOG:
		g_pcTargetFile->setParent(hwndSectionEditorDlg);

		g_pcTargetFile->pDebugConsol->PrintDebugString("");
		#ifdef DEBUGMODE
		printf("IDC_LIST_SECTIONSEDITOR::VM_NOTIFY::%d\n", *g_pSelectedRow);
		#endif // DEBUGMODE

		showSectionHeaderInfo(*g_pSelectedRow);
		return false;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_BUTTON_SECTIONEDITOR_CLOSE:
			g_pcTargetFile->pDebugConsol->PrintDebugString("IDC_BUTTON_SECTIONEDITOR_CLOSE (Close)\n");
			EndDialog(hwndSectionEditorDlg, 0);
			return false;

		default:
			return false;
		}

	default:
		return false;
	}
}

void SectionEditor(HINSTANCE hImageBase, HWND hwndListControl, int selectedRow) {
	g_pSelectedRow = new int;
	*g_pSelectedRow = selectedRow;

	DialogBox(hImageBase, MAKEINTRESOURCE(IDD_DIALOG_SECTIONEDITOR), hwndListControl, CallBack_SectionEditorDlg);
}

bool showSectionHeaderInfo(int selectedRow) {
	unsigned char* pSectionHeader_Temp = (unsigned char*)g_pcTargetFile->pcSectionHeader;
	pSectionHeader_Temp += 0x28 * selectedRow;
	char sectionsHeaderSize[10] = { 8, 4, 4, 4, 4, 4, 4, 2, 2, 4 };

	int L = 1;
	int editControlOffset = 1;
	unsigned char szHex[11] = { "0x" };

	HWND hwnd = g_pcTargetFile->getCallerHwnd();
	FILE* fpOrignal = g_pcTargetFile->getFilePtr();

	char szNameBuffer[9] = { 0 };
	memcpy(szNameBuffer, pSectionHeader_Temp, 8);

	HWND hEditControl = GetDlgItem(hwnd, IDC_EDIT_NAME);
	SetWindowText(hEditControl, szNameBuffer);
	pSectionHeader_Temp += 0x08;
	do {
		ASCIItoHex((unsigned char*)pSectionHeader_Temp, szHex + 2, sectionsHeaderSize[L]);
		hEditControl = GetDlgItem(hwnd, IDC_EDIT_NAME + editControlOffset++);
		SetWindowText(hEditControl, (char*)szHex);
		memset(szHex + 2, 0, 9);
		pSectionHeader_Temp += sectionsHeaderSize[L];
		L++;
	} while (L != 10);
	fseek(fpOrignal, 0, SEEK_SET);

	free(g_pSelectedRow);
	return true;
}