#include "BaseRelocationDirectoryDlg.h"

bool initBaseRelocationDirectoryListControl(HWND hwndBaseRelocationDirectoryDirectoryDlg);
void showBlockDetail(HWND hwndListControlBaseRelocationDirectory, WPARAM wParam, LPARAM lParam);

extern TargetFile* g_pcTargetFile;
typedef unsigned char* PBASERELOCATION;

BOOL CALLBACK CallBack_BaseRelocationDirectoryDlg(
	HWND hwndBaseRelocationDirectoryDirectoryDlg,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
) {
	switch (uMsg) {
	case WM_INITDIALOG:
		g_pcTargetFile->setParent(hwndBaseRelocationDirectoryDirectoryDlg);
		initBaseRelocationDirectoryListControl(hwndBaseRelocationDirectoryDirectoryDlg);
		return false;

	case WM_NOTIFY: {
		NMHDR* pNMHDR = (NMHDR*)lParam;
		if (wParam == IDC_LIST_BASERELOCATIONDIRECTORY && pNMHDR->code == NM_CLICK)
			showBlockDetail(GetDlgItem(hwndBaseRelocationDirectoryDirectoryDlg, IDC_LIST_BASERELOCATIONDIRECTORY), wParam, lParam);

		return false;
	}

	case WM_COMMAND:

		switch (LOWORD(wParam)) {
		case IDC_BUTTON_BASERELOCATIONDIRECTORY_CLOSE:
			g_pcTargetFile->pDebugConsol->PrintDebugString("IDC_BUTTON_IMPORTDIRECTORY_CLOSE (Close)\n");
			EndDialog(hwndBaseRelocationDirectoryDirectoryDlg, 0);
			return false;

		default:
			return false;
		}

	default:
		return false;
	}
}

void BaseRelocationDirectory(HINSTANCE hImageBase, HWND hwndDirectoriesDlg) {
	DialogBox(hImageBase, MAKEINTRESOURCE(IDD_DIALOG_BASERELOCATIONDIRECTORY), hwndDirectoriesDlg, CallBack_BaseRelocationDirectoryDlg);
}

bool initBaseRelocationDirectoryListControl(HWND hwndBaseRelocationDirectoryDlg) {
	LV_COLUMN lvcolum;
	HWND hwndListControl;
	memset(&lvcolum, 0, sizeof(LV_COLUMN));

	hwndListControl = GetDlgItem(hwndBaseRelocationDirectoryDlg, IDC_LIST_BASERELOCATIONDIRECTORY);

	// Full row seclect
	SendMessage(hwndListControl, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	// Can reduce code by using string table
	lvcolum.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvcolum.pszText = (LPSTR)"ID";
	lvcolum.cx = 40;
	lvcolum.iSubItem = 0;
	SendMessage(hwndListControl, LVM_INSERTCOLUMN, 0, (DWORD)&lvcolum);

	lvcolum.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvcolum.pszText = (LPSTR)"Section";
	lvcolum.cx = 60;
	lvcolum.iSubItem = 1;
	SendMessage(hwndListControl, LVM_INSERTCOLUMN, 1, (DWORD)&lvcolum);

	lvcolum.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvcolum.pszText = (LPSTR)"RVA";
	lvcolum.cx = 80;
	lvcolum.iSubItem = 2;
	SendMessage(hwndListControl, LVM_INSERTCOLUMN, 2, (DWORD)&lvcolum);

	lvcolum.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvcolum.pszText = (LPSTR)"Size Of Block";
	lvcolum.cx = 80;
	lvcolum.iSubItem = 3;
	SendMessage(hwndListControl, LVM_INSERTCOLUMN, 3, (DWORD)&lvcolum);

	lvcolum.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvcolum.pszText = (LPSTR)"Items";
	lvcolum.cx = 40;
	lvcolum.iSubItem = 4;
	SendMessage(hwndListControl, LVM_INSERTCOLUMN, 4, (DWORD)&lvcolum);

	// Init Block Items Head
	hwndListControl = GetDlgItem(hwndBaseRelocationDirectoryDlg, IDC_LIST_BASERELOCATIONDIRECTORY_BLOCKITEM);
	SendMessage(hwndListControl, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	lvcolum.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvcolum.pszText = (LPSTR)"ID";
	lvcolum.cx = 40;
	lvcolum.iSubItem = 0;
	SendMessage(hwndListControl, LVM_INSERTCOLUMN, 0, (DWORD)&lvcolum);

	lvcolum.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvcolum.pszText = (LPSTR)"RVA";
	lvcolum.cx = 80;
	lvcolum.iSubItem = 1;
	SendMessage(hwndListControl, LVM_INSERTCOLUMN, 1, (DWORD)&lvcolum);

	lvcolum.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvcolum.pszText = (LPSTR)"Offset";
	lvcolum.cx = 80;
	lvcolum.iSubItem = 2;
	SendMessage(hwndListControl, LVM_INSERTCOLUMN, 2, (DWORD)&lvcolum);

	lvcolum.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvcolum.pszText = (LPSTR)"Type";
	lvcolum.cx = 40;
	lvcolum.iSubItem = 3;
	SendMessage(hwndListControl, LVM_INSERTCOLUMN, 3, (DWORD)&lvcolum);

	// Init Blocks
	LV_ITEM lvitem;
	memset(&lvitem, 0, sizeof(LV_ITEM));

	// Get number of Image base relocation.
	Tools* pcTools = new Tools(g_pcTargetFile);
	IMAGE_DATA_DIRECTORY* pcDirectoryEntryBaseReloc = g_pcTargetFile->pcDataDirectories + 5;
	hwndListControl = GetDlgItem(hwndBaseRelocationDirectoryDlg, IDC_LIST_BASERELOCATIONDIRECTORY);
	unsigned int pImageBaseRelocationDirectoryFOA = NULL;
	PIMAGEBUFFER pImageBuffer_Temp = g_pcTargetFile->getImageBuffer();
	pcTools->RvaToFoa(pcDirectoryEntryBaseReloc->VirtualAddress, &pImageBaseRelocationDirectoryFOA, g_pcTargetFile);
	PBASERELOCATION pImageBuffer_BaseRelocationDirectory = pImageBuffer_Temp + pImageBaseRelocationDirectoryFOA;

	char szSectionNameBuffer[9] = { 0 };
	char szIdBuffer[5] = { 0 };
	unsigned char szHex[11] = { "0x" };
	unsigned int numberOfItem = 0;

	unsigned int L = 0;
	do {
		sprintf(szIdBuffer, "%04d", L + 1);
		lvitem.pszText = szIdBuffer;
		lvitem.iItem = L;
		lvitem.iSubItem = 0;
		lvitem.mask = LVIF_TEXT;
		ListView_InsertItem(hwndListControl, &lvitem);
		memset(szIdBuffer, 0, 5);

		pcTools->getSectionName(*((unsigned int*)(pImageBuffer_BaseRelocationDirectory) + 0), szSectionNameBuffer, g_pcTargetFile);
		lvitem.pszText = szSectionNameBuffer;
		lvitem.iItem = L;
		lvitem.iSubItem = 1;
		lvitem.mask = LVIF_TEXT;
		ListView_SetItem(hwndListControl, &lvitem);
		memset(szSectionNameBuffer, 0, 9);

		ASCIItoHex(pImageBuffer_BaseRelocationDirectory, szHex + 2, 4);
		lvitem.pszText = (LPSTR)szHex;
		lvitem.iItem = L;
		lvitem.iSubItem = 2;
		lvitem.mask = LVIF_TEXT;
		ListView_SetItem(hwndListControl, &lvitem);
		memset(szHex + 2, 0, 9);
	
		ASCIItoHex(pImageBuffer_BaseRelocationDirectory + 0x04, szHex + 2, 4);
		lvitem.pszText = (LPSTR)szHex;
		lvitem.iItem = L;
		lvitem.iSubItem = 3;
		lvitem.mask = LVIF_TEXT;
		ListView_SetItem(hwndListControl, &lvitem);
		memset(szHex + 2, 0, 9);

		numberOfItem = (*((unsigned int*)(pImageBuffer_BaseRelocationDirectory + 0x04) + 0) - 0x08) / 0x02;
		ASCIItoHex((unsigned char*)&numberOfItem, szHex, 2);
		lvitem.pszText = (LPSTR)szHex;
		lvitem.iItem = L;
		lvitem.iSubItem = 4;
		lvitem.mask = LVIF_TEXT;
		ListView_SetItem(hwndListControl, &lvitem);
		pImageBuffer_BaseRelocationDirectory += *((unsigned int*)(pImageBuffer_BaseRelocationDirectory + 0x04) + 0);
		memset(szSectionNameBuffer, 0, 9);

		L++;
	} while (*((unsigned int*)(pImageBuffer_BaseRelocationDirectory)+0) + *((unsigned int*)(pImageBuffer_BaseRelocationDirectory + 0x04) + 0));

	free(pcTools);
	return true;
}

void showBlockDetail(HWND hwndListControlBaseRelocationDirectory, WPARAM wParam, LPARAM lParam) {
	int selectedRow = 0;
	selectedRow = SendMessage(hwndListControlBaseRelocationDirectory, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
	SendMessage(hwndListControlBaseRelocationDirectory, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
	if (selectedRow == -1)
		return;

	LV_ITEM lvitem;
	memset(&lvitem, 0, sizeof(LV_ITEM));

	HWND hwndListControl = GetDlgItem(g_pcTargetFile->getCallerHwnd(), IDC_LIST_BASERELOCATIONDIRECTORY_BLOCKITEM);
	ListView_DeleteAllItems(hwndListControl);
	Tools* pcTools = new Tools(g_pcTargetFile);
	IMAGE_DATA_DIRECTORY* pcDirectoryEntryBaseReloc = g_pcTargetFile->pcDataDirectories + 5;
	PIMAGEBUFFER pImageBuffer_Temp = g_pcTargetFile->getImageBuffer();
	unsigned int pImageBaseRelocationDirectoryFOA = NULL;
	pcTools->RvaToFoa(pcDirectoryEntryBaseReloc->VirtualAddress, &pImageBaseRelocationDirectoryFOA, g_pcTargetFile);
	PBASERELOCATION pImageBuffer_BaseRelocationDirectory = pImageBuffer_Temp + pImageBaseRelocationDirectoryFOA;

	char szIdBuffer[5] = { 0 };
	unsigned char szHex[11] = { "0x" };
	unsigned int numberOfItem = 0;
	unsigned int L = 1;

	unsigned int blockItemType = 0;
	unsigned int blockBaseRVA = *((unsigned int*)(pImageBuffer_BaseRelocationDirectory) + 0);
	unsigned int blockItemRVA = 0;
	unsigned int blockItemFOA = 0;
	unsigned int blockSize = *((unsigned int*)(pImageBuffer_BaseRelocationDirectory + 0x04) + 0);

	while (selectedRow != 0) {
		unsigned int blockSize = *((unsigned int*)(pImageBuffer_BaseRelocationDirectory + 0x04) + 0);
		pImageBuffer_BaseRelocationDirectory += blockSize;
		selectedRow--;
	}
	blockBaseRVA = *((unsigned int*)(pImageBuffer_BaseRelocationDirectory) + 0);
	blockSize = *((unsigned int*)(pImageBuffer_BaseRelocationDirectory + 0x04) + 0);

	numberOfItem = (*((unsigned int*)(pImageBuffer_BaseRelocationDirectory + 0x04) + 0) - 0x08) / 0x02;
	pImageBuffer_BaseRelocationDirectory += 0x08;
	do {
		sprintf(szIdBuffer, "%04d", L);
		lvitem.pszText = (LPSTR)szIdBuffer;
		lvitem.iItem = L - 1;
		lvitem.iSubItem = 0;
		lvitem.mask = LVIF_TEXT;
		ListView_InsertItem(hwndListControl, &lvitem);
		memset(szIdBuffer, 0, 5);

		blockItemType = (*((unsigned short*)pImageBuffer_BaseRelocationDirectory + 0) & 0xF000) >> 12;
		blockItemRVA = *((unsigned short*)pImageBuffer_BaseRelocationDirectory + 0) & 0x0FFF;
		blockItemRVA += blockBaseRVA;
		for (unsigned int i = 1; i <= 3; i++) {
			switch (i) {
			case 1:
				if (!blockItemType) {
					strcpy((char*)szHex, "N/A");
				} else
					ASCIItoHex((unsigned char*)&blockItemRVA, szHex + 2, 4);
				
				lvitem.iSubItem = 1;
				break;
			case 2:
				if (!blockItemType) {
					strcpy((char*)szHex, "N/A");
				} else {
					pcTools->RvaToFoa(blockItemRVA, &blockItemFOA, g_pcTargetFile);
					ASCIItoHex((unsigned char*)&blockItemFOA, szHex + 2, 4);
				}

				lvitem.iSubItem = 2;
				break;
			case 3:
				ASCIItoHex((unsigned char*)&blockItemType, szHex + 2, 1);
				lvitem.iSubItem = 3;
				break;
			}
			lvitem.pszText = (LPSTR)szHex;
			lvitem.iItem = L - 1;
			lvitem.mask = LVIF_TEXT;
			ListView_SetItem(hwndListControl, &lvitem);
			strcpy((char*)szHex, "0x");
			memset(szHex + 2, 0, 9);
		}
		pImageBuffer_BaseRelocationDirectory += 0x02;
	} while (L++ != numberOfItem);

	free(pcTools);
}