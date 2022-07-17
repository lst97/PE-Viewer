#include "SectionsHeaderDlg.h"

bool initSectionsListControl(HWND hwndSectionsHeaderDlg);
bool EditSectionsItem(HWND hwndListControl, WPARAM wParam, LPARAM lParam);
void SectionEditor(HINSTANCE hImageBase, HWND hwndListControl, int selectedRow);

extern TargetFile* g_pcTargetFile;

BOOL CALLBACK CallBack_SectionsHeaderDlg(
	HWND hwndSectionsHeaderDlg,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
) {
	switch (uMsg) {
	case WM_INITDIALOG:
		g_pcTargetFile->setParent(hwndSectionsHeaderDlg);
		initSectionsListControl(hwndSectionsHeaderDlg);
		return false;

	case WM_NOTIFY: {
		NMHDR* pNMHDR = (NMHDR*)lParam;
		if (wParam == IDC_LIST_SECTIONS && pNMHDR->code == NM_DBLCLK) 
			EditSectionsItem(GetDlgItem(hwndSectionsHeaderDlg, IDC_LIST_SECTIONS), wParam, lParam);
		
		return false;
	}

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_BUTTON_SECTIONS_CLOSE:
			g_pcTargetFile->pDebugConsol->PrintDebugString("IDC_BUTTON_SECTIONS_CLOSE (Close)\n");
			EndDialog(hwndSectionsHeaderDlg, 0);
			return false;
		default:
			return false;
		}

	default:
		return false;
	}
}

void SectionsHeader(HINSTANCE hImageBase, HWND hwndPeEditorDlg) {
	DialogBox(hImageBase, MAKEINTRESOURCE(IDD_DIALOG_SECTIONS), hwndPeEditorDlg, CallBack_SectionsHeaderDlg);
}

bool initSectionsListControl(HWND hwndSectionsHeaderDlg) {
	LV_COLUMN lvcolum;
	HWND hwndListControl;
	memset(&lvcolum, 0, sizeof(LV_COLUMN));

	hwndListControl = GetDlgItem(hwndSectionsHeaderDlg, IDC_LIST_SECTIONS);

	// Full row seclect
	SendMessage(hwndListControl, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	// Can reduce code by using string table
	lvcolum.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvcolum.pszText = (LPSTR)"ID";
	lvcolum.cx = 30;
	lvcolum.iSubItem = 0;
	SendMessage(hwndListControl, LVM_INSERTCOLUMN, 0, (DWORD)&lvcolum);

	lvcolum.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvcolum.pszText = (LPSTR)"Name";
	lvcolum.cx = 70;
	lvcolum.iSubItem = 1;
	SendMessage(hwndListControl, LVM_INSERTCOLUMN, 1, (DWORD)&lvcolum);

	lvcolum.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvcolum.pszText = (LPSTR)"Virtual Size";
	lvcolum.cx = 80;
	lvcolum.iSubItem = 2;
	SendMessage(hwndListControl, LVM_INSERTCOLUMN, 2, (DWORD)&lvcolum);

	lvcolum.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvcolum.pszText = (LPSTR)"Virtual Address";
	lvcolum.cx = 90;
	lvcolum.iSubItem = 3;
	SendMessage(hwndListControl, LVM_INSERTCOLUMN, 3, (DWORD)&lvcolum);

	lvcolum.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvcolum.pszText = (LPSTR)"Size Of Raw Data";
	lvcolum.cx = 100;
	lvcolum.iSubItem = 4;
	SendMessage(hwndListControl, LVM_INSERTCOLUMN, 4, (DWORD)&lvcolum);

	lvcolum.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvcolum.pszText = (LPSTR)"Pointer To Raw Data";
	lvcolum.cx = 120;
	lvcolum.iSubItem = 5;
	SendMessage(hwndListControl, LVM_INSERTCOLUMN, 5, (DWORD)&lvcolum);

	lvcolum.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvcolum.pszText = (LPSTR)"Characteristics";
	lvcolum.cx = 90;
	lvcolum.iSubItem = 6;
	SendMessage(hwndListControl, LVM_INSERTCOLUMN, 6, (DWORD)&lvcolum);

	// Init item
	LV_ITEM lvitem;
	memset(&lvitem, 0, sizeof(LV_ITEM));

	lvitem.mask = LVIF_TEXT;
	unsigned char* pSectionHeader_Temp = (unsigned char*)g_pcTargetFile->pcSectionHeader;
	pSectionHeader_Temp += 0x28 * (g_pcTargetFile->pcFileHeader->NumberOfSections -1);

	char szIdBuffer[3];
	char szItemBuffer[11] = { "0x" };
	char szNameBuffer[9] = { 0 };

	for (int i = 0; i < g_pcTargetFile->pcFileHeader->NumberOfSections; i++) {
		sprintf(szIdBuffer, "%02d", (g_pcTargetFile->pcFileHeader->NumberOfSections - i));
		lvitem.pszText = szIdBuffer;
		lvitem.iItem = 0;
		lvitem.iSubItem = 0;
		ListView_InsertItem(hwndListControl, &lvitem);

		for (int j = 1; j <= 5; j++) {
			if (j == 1) {
				memcpy(szNameBuffer, pSectionHeader_Temp, 8);
				lvitem.pszText = szNameBuffer;
			}
			else {
				ASCIItoHex((pSectionHeader_Temp + j * 4), (unsigned char*)szItemBuffer + 2, 4);
				lvitem.pszText = szItemBuffer;
			}
			lvitem.iItem = 0;
			lvitem.iSubItem = j;
			ListView_SetItem(hwndListControl, &lvitem);
		}
		ASCIItoHex((pSectionHeader_Temp + 0x24), (unsigned char*)szItemBuffer + 2, 4);
		lvitem.pszText = szItemBuffer;
		lvitem.iItem = 0;
		lvitem.iSubItem = 6;
		ListView_SetItem(hwndListControl, &lvitem);

		pSectionHeader_Temp -= 0x28;
	}

	return true;
}

bool EditSectionsItem(HWND hwndListControl, WPARAM wParam, LPARAM lParam) {
	int selectedRow = 0;

	selectedRow = SendMessage(hwndListControl, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
	if (selectedRow == -1)
		return false;

	HINSTANCE hImageBase = g_pcTargetFile->getImageBase();

	SectionEditor(hImageBase, hwndListControl, selectedRow);
	
	return true;
}