#include "ImportDirectoriesDlg.h"

bool initImportDirectoryListControl(HWND hwndImportDirectoryDlg);
unsigned int getLonggestDllNameLength(IMAGE_IMPORT_DESCRIPTOR* pcImportDescriptor, unsigned int numberOfImportDescriptor);
void showDllDetail(HWND hwndListControlImportDirectory, WPARAM wParam, LPARAM lParam);

extern TargetFile* g_pcTargetFile;

BOOL CALLBACK CallBack_ImportDirectoryDlg(
	HWND hwndImportDirectoryDlg,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
) {
	switch (uMsg) {
	case WM_INITDIALOG:
		g_pcTargetFile->setParent(hwndImportDirectoryDlg);
		initImportDirectoryListControl(hwndImportDirectoryDlg);

		return false;

	case WM_NOTIFY: {
		NMHDR* pNMHDR = (NMHDR*)lParam;
		if (wParam == IDC_LIST_IMPORTDIRECTORY && pNMHDR->code == NM_CLICK) 
			showDllDetail(GetDlgItem(hwndImportDirectoryDlg, IDC_LIST_IMPORTDIRECTORY), wParam, lParam);
		
		return false;
	}

	case WM_COMMAND:

		switch (LOWORD(wParam)) {
		case IDC_BUTTON_IMPORTDIRECTORY_CLOSE:
			g_pcTargetFile->pDebugConsol->PrintDebugString("IDC_BUTTON_IMPORTDIRECTORY_CLOSE (Close)\n");
			EndDialog(hwndImportDirectoryDlg, 0);
			return false;

		default:
			return false;
		}

	default:
		return false;
	}
}

void ImportDirectory(HINSTANCE hImageBase, HWND hwndDirectoriesDlg) {
	DialogBox(hImageBase, MAKEINTRESOURCE(IDD_DIALOG_IMPORTDIRECTORY), hwndDirectoriesDlg, CallBack_ImportDirectoryDlg);
}

bool initImportDirectoryListControl(HWND hwndImportDirectoryDlg) {
	LV_COLUMN lvcolum;
	HWND hwndListControl;
	memset(&lvcolum, 0, sizeof(LV_COLUMN));

	hwndListControl = GetDlgItem(hwndImportDirectoryDlg, IDC_LIST_IMPORTDIRECTORY);

	// Full row seclect
	SendMessage(hwndListControl, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	// Can reduce code by using string table
	lvcolum.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvcolum.pszText = (LPSTR)"DLL Name";
	lvcolum.cx = 120;
	lvcolum.iSubItem = 0;
	SendMessage(hwndListControl, LVM_INSERTCOLUMN, 0, (DWORD)&lvcolum);

	lvcolum.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvcolum.pszText = (LPSTR)"Orignal First Thunk";
	lvcolum.cx = 110;
	lvcolum.iSubItem = 1;
	SendMessage(hwndListControl, LVM_INSERTCOLUMN, 1, (DWORD)&lvcolum);

	lvcolum.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvcolum.pszText = (LPSTR)"Time/Date Stamp";
	lvcolum.cx = 100;
	lvcolum.iSubItem = 2;
	SendMessage(hwndListControl, LVM_INSERTCOLUMN, 2, (DWORD)&lvcolum);

	lvcolum.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvcolum.pszText = (LPSTR)"Forwarder Chain";
	lvcolum.cx = 100;
	lvcolum.iSubItem = 3;
	SendMessage(hwndListControl, LVM_INSERTCOLUMN, 3, (DWORD)&lvcolum);

	lvcolum.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvcolum.pszText = (LPSTR)"Name";
	lvcolum.cx = 80;
	lvcolum.iSubItem = 4;
	SendMessage(hwndListControl, LVM_INSERTCOLUMN, 4, (DWORD)&lvcolum);

	lvcolum.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvcolum.pszText = (LPSTR)"First Thunk";
	lvcolum.cx = 80;
	lvcolum.iSubItem = 5;
	SendMessage(hwndListControl, LVM_INSERTCOLUMN, 5, (DWORD)&lvcolum);

	// Init Detail
	hwndListControl = GetDlgItem(hwndImportDirectoryDlg, IDC_LIST_IMPORTDIRECTORY_DETAIL);
	SendMessage(hwndListControl, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	lvcolum.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvcolum.pszText = (LPSTR)"Function Name";
	lvcolum.cx = 270;
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
	lvcolum.pszText = (LPSTR)"Value";
	lvcolum.cx = 80;
	lvcolum.iSubItem = 3;
	SendMessage(hwndListControl, LVM_INSERTCOLUMN, 3, (DWORD)&lvcolum);

	lvcolum.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvcolum.pszText = (LPSTR)"Hint";
	lvcolum.cx = 80;
	lvcolum.iSubItem = 4;
	SendMessage(hwndListControl, LVM_INSERTCOLUMN, 4, (DWORD)&lvcolum);

	// Init item
	LV_ITEM lvitem;
	memset(&lvitem, 0, sizeof(LV_ITEM));

	hwndListControl = GetDlgItem(hwndImportDirectoryDlg, IDC_LIST_IMPORTDIRECTORY);

	lvitem.mask = LVIF_TEXT;
	IMAGE_IMPORT_DESCRIPTOR* pcImportDescriptor_Temp = g_pcTargetFile->pcImportDescriptor;

	unsigned char szItemBuffer[11] = { "0x" };
	unsigned int dllNameBufferSize = getLonggestDllNameLength(pcImportDescriptor_Temp, g_pcTargetFile->getNumberOfImportDescriptor());
	char* pszDllNameBuffer = new char[dllNameBufferSize];
	memset(pszDllNameBuffer, 0, dllNameBufferSize);
	unsigned int numberOfImportDescriptor = g_pcTargetFile->getNumberOfImportDescriptor();
	Tools* pcTools = new Tools(g_pcTargetFile);
	unsigned int dllNameFOA = 0;
	char* pImageBuffer_Temp = (char*)g_pcTargetFile->getImageBuffer();

	for (unsigned int i = 0; i < numberOfImportDescriptor; i++) {
		pcTools->RvaToFoa(pcImportDescriptor_Temp->Name, &dllNameFOA, g_pcTargetFile);
		strcpy(pszDllNameBuffer, pImageBuffer_Temp + dllNameFOA);
		lvitem.pszText = pszDllNameBuffer;
		lvitem.iItem = i;
		lvitem.iSubItem = 0;
		ListView_InsertItem(hwndListControl, &lvitem);
		memset(pszDllNameBuffer, 0, dllNameBufferSize);

		for (unsigned int j = 1; j <= 5; j++) {
			ASCIItoHex((unsigned char*)((unsigned char*)pcImportDescriptor_Temp + 4 * (j - 1)), szItemBuffer + 2, 4);
			lvitem.pszText = (LPSTR)szItemBuffer;
			lvitem.iItem = i;
			lvitem.iSubItem = j;
			ListView_SetItem(hwndListControl, &lvitem);
			memset(szItemBuffer + 2, 0, 9);
		}
		pcImportDescriptor_Temp++;
	}

	return true;
}

unsigned int getLonggestDllNameLength(IMAGE_IMPORT_DESCRIPTOR* pcImportDescriptor, unsigned int numberOfImportDescriptor) {
	char* pszDllName = NULL;
	unsigned int nameLengthMAX = 0;
	unsigned int nameLength = 0;
	unsigned int L = numberOfImportDescriptor;
	unsigned int dllNameFOA = NULL;
	PIMAGEBUFFER pImageBuffer_Temp = g_pcTargetFile->getImageBuffer();
	Tools* pcTools = new Tools(g_pcTargetFile);
	IMAGE_IMPORT_DESCRIPTOR* pcImportDescriptor_Temp = pcImportDescriptor;

	do {
		pcTools->RvaToFoa(pcImportDescriptor_Temp->Name, &dllNameFOA, g_pcTargetFile);
		pcImportDescriptor_Temp++;
		pszDllName = (char*)pImageBuffer_Temp + dllNameFOA;
		nameLength = strlen(pszDllName);
		if(nameLength > nameLengthMAX)
			nameLengthMAX = nameLength;
		L--;
	} while (L != 0);

	free(pcTools);
	return nameLengthMAX + 1;
}

void showDllDetail(HWND hwndListControlImportDirectory, WPARAM wParam, LPARAM lParam) {
	int selectedRow = 0;
	selectedRow = SendMessage(hwndListControlImportDirectory, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
	if (selectedRow == -1)
		return;

	Tools* pcTools = new Tools(g_pcTargetFile);
	unsigned char* pImageBuffer_Temp = g_pcTargetFile->getImageBuffer();

	IMAGE_IMPORT_DESCRIPTOR* pcImportDescriptor_Temp = g_pcTargetFile->pcImportDescriptor;
	pcImportDescriptor_Temp += selectedRow;

	unsigned int originalFirstThunk_Temp = pcImportDescriptor_Temp->OriginalFirstThunk;
	unsigned int imageThunkDataFOA = 0;
	unsigned int imageImportByNameFOA = 0;
	unsigned int numberofThunkData = 0;
	pcTools->RvaToFoa(pcImportDescriptor_Temp->OriginalFirstThunk, &imageThunkDataFOA, g_pcTargetFile);

	LV_ITEM lvitem;
	memset(&lvitem, 0, sizeof(LV_ITEM));

	HWND hwndListControlImportDirectoryDetail = GetDlgItem(g_pcTargetFile->getCallerHwnd(), IDC_LIST_IMPORTDIRECTORY_DETAIL);
	unsigned int L = 0;
	unsigned int pFunctionNameFOA = NULL;
	char* pFunctionName = NULL;

	unsigned char szHex[11] = { "0x" };
	lvitem.mask = LVIF_TEXT;

	do {
		imageThunkDataFOA += 4;
		numberofThunkData++;
	} while (*(unsigned int*)(pImageBuffer_Temp + imageThunkDataFOA) != 0);
	imageThunkDataFOA -= 4;
	numberofThunkData--;

	ListView_DeleteAllItems(hwndListControlImportDirectoryDetail);
	do {
		if (*(unsigned int*)(pImageBuffer_Temp + imageThunkDataFOA) & 0x80000000) {
			// Function import as index

		} else {
			pcTools->RvaToFoa(*(unsigned int*)(pImageBuffer_Temp + imageThunkDataFOA), &imageImportByNameFOA, g_pcTargetFile);
			lvitem.pszText = (LPSTR)(pImageBuffer_Temp + imageImportByNameFOA + 0x02);
			lvitem.iItem = L;
			lvitem.iSubItem = 0;
			ListView_InsertItem(hwndListControlImportDirectoryDetail, &lvitem);

			for (unsigned int i = 1; i <= 4; i++) {
				switch (i) {
				case 1:
					ASCIItoHex((unsigned char*)&originalFirstThunk_Temp, szHex + 2, 4);
					break;
				case 2:
					ASCIItoHex((unsigned char*)&imageThunkDataFOA, szHex + 2, 4);
					break;
				case 3:
					ASCIItoHex((unsigned char*)(pImageBuffer_Temp + imageThunkDataFOA), szHex + 2, 4);
					break;
				case 4:
					ASCIItoHex((unsigned char*)(pImageBuffer_Temp + imageImportByNameFOA), szHex + 2, 2);
					break;
				}
				lvitem.pszText = (LPSTR)szHex;
				lvitem.iItem = L;
				lvitem.iSubItem = i;
				ListView_SetItem(hwndListControlImportDirectoryDetail, &lvitem);
				memset(szHex + 2, 0, 9);
			}
		}
		if (numberofThunkData) {
			originalFirstThunk_Temp -= 4;
			imageThunkDataFOA -= 4;
		}
	} while (numberofThunkData-- != 0);

	free(pcTools);

}