#include "ExportDirectoryDlg.h"

bool initExportDirectory(HWND hwndExportDirectoryDirectoryDlg);
bool initExportDirectoryListControl(HWND hwndExportDirectoryDirectoryDlg);
void showFunctionDetail(HWND hwndExportDirectoryDirectoryDlg);

extern TargetFile* g_pcTargetFile;

BOOL CALLBACK CallBack_ExportDirectoryDlg(
	HWND hwndExportDirectoryDirectoryDlg,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
) {
	switch (uMsg) {
	case WM_INITDIALOG:
		g_pcTargetFile->setParent(hwndExportDirectoryDirectoryDlg);
		initExportDirectory(hwndExportDirectoryDirectoryDlg);
		initExportDirectoryListControl(hwndExportDirectoryDirectoryDlg);
		showFunctionDetail(hwndExportDirectoryDirectoryDlg);
		return false;

	case WM_COMMAND:

		switch (LOWORD(wParam)) {
		case IDC_BUTTON_EXPORTDIRECTORY_CLOSE:
			g_pcTargetFile->pDebugConsol->PrintDebugString("IDC_BUTTON_EXPORTDIRECTORY_CLOSE (Close)\n");
			EndDialog(hwndExportDirectoryDirectoryDlg, 0);
			return false;

		default:
			return false;
		}

	default:
		return false;
	}
}

void ExportDirectory(HINSTANCE hImageBase, HWND hwndDirectoriesDlg) {
	DialogBox(hImageBase, MAKEINTRESOURCE(IDD_DIALOG_EXPORTDIRECTORY), hwndDirectoriesDlg, CallBack_ExportDirectoryDlg);
}

bool initExportDirectory(HWND hwndExportDirectoryDirectoryDlg) {

	unsigned char exportDitectorySize[11] = { 4, 4, 2, 2, 4, 4, 4, 4, 4, 4, 4 };
	Tools *pcTools = new Tools(g_pcTargetFile);

	PIMAGEBUFFER pExportDirectory = (PIMAGEBUFFER)g_pcTargetFile->pcExportDirectory;
	PIMAGEBUFFER pImageBuffer_Temp = g_pcTargetFile->getImageBuffer();
	HWND hwndEditControl = NULL;
	unsigned int L = 0;
	unsigned char szHex[11] = { "0x" };
	unsigned int dllNameFOA = 0;
	pcTools->RvaToFoa(*((unsigned int*)(pExportDirectory + 0x0C) + 0), &dllNameFOA, g_pcTargetFile);
	do {
		hwndEditControl = GetDlgItem(hwndExportDirectoryDirectoryDlg, IDC_EDIT_EXPORTDIRECTORY_CHARACTERISTICS + L);
		ASCIItoHex(pExportDirectory, szHex + 2, exportDitectorySize[L]);
		if (L != 4)
			SetWindowText(hwndEditControl, (LPSTR)szHex);
		else
			SetWindowText(hwndEditControl, (LPSTR)(pImageBuffer_Temp + dllNameFOA));
		memset(szHex + 2, 0, 9);
		pExportDirectory += exportDitectorySize[L];
	} while (++L != 11);

	free(pcTools);
	return true;
}

bool initExportDirectoryListControl(HWND hwndExportDirectoryDirectoryDlg) {
	LV_COLUMN lvcolum;
	HWND hwndListControl;
	memset(&lvcolum, 0, sizeof(LV_COLUMN));

	hwndListControl = GetDlgItem(hwndExportDirectoryDirectoryDlg, IDC_LIST_EXPORTDIRECTORY);

	// Full row seclect
	SendMessage(hwndListControl, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	// Can reduce code by using string table
	lvcolum.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvcolum.pszText = (LPSTR)"Ordinal";
	lvcolum.cx = 50;
	lvcolum.iSubItem = 0;
	SendMessage(hwndListControl, LVM_INSERTCOLUMN, 0, (DWORD)&lvcolum);

	lvcolum.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvcolum.pszText = (LPSTR)"RVA";
	lvcolum.cx = 80;
	lvcolum.iSubItem = 0;
	SendMessage(hwndListControl, LVM_INSERTCOLUMN, 1, (DWORD)&lvcolum);

	lvcolum.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvcolum.pszText = (LPSTR)"FOA";
	lvcolum.cx = 80;
	lvcolum.iSubItem = 0;
	SendMessage(hwndListControl, LVM_INSERTCOLUMN, 2, (DWORD)&lvcolum);

	lvcolum.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvcolum.pszText = (LPSTR)"Function Name";
	lvcolum.cx = 291;
	lvcolum.iSubItem = 0;
	SendMessage(hwndListControl, LVM_INSERTCOLUMN, 3, (DWORD)&lvcolum);

	return true;
}

void showFunctionDetail(HWND hwndExportDirectoryDirectoryDlg) {

	Tools *pcTools = new Tools(g_pcTargetFile);

	unsigned int addressOfFunctionsFOA = 0;
	pcTools->RvaToFoa(g_pcTargetFile->pcExportDirectory->AddressOfFunctions, &addressOfFunctionsFOA, g_pcTargetFile);
	unsigned int addressOfNamesFOA = 0;
	pcTools->RvaToFoa(g_pcTargetFile->pcExportDirectory->AddressOfNames, &addressOfNamesFOA, g_pcTargetFile);
	unsigned int addressOfNameOrdinalsFOA = 0;
	pcTools->RvaToFoa(g_pcTargetFile->pcExportDirectory->AddressOfNameOrdinals, &addressOfNameOrdinalsFOA, g_pcTargetFile);
	unsigned int numberOfFunctions = g_pcTargetFile->pcExportDirectory->NumberOfFunctions;
	unsigned int numberOfNames = g_pcTargetFile->pcExportDirectory->NumberOfNames;
	unsigned int base = g_pcTargetFile->pcExportDirectory->Base;
	unsigned int ordinal = 0;
	unsigned int functionFOA = 0;
	unsigned int nameFOA = 0;
	PIMAGEBUFFER pImageBuffer_Functions = g_pcTargetFile->getImageBuffer() + addressOfFunctionsFOA;
	PIMAGEBUFFER pImageBuffer_Names = g_pcTargetFile->getImageBuffer() + addressOfNamesFOA;
	PIMAGEBUFFER pImageBuffer_Ordinals = g_pcTargetFile->getImageBuffer() + addressOfNameOrdinalsFOA;
	PIMAGEBUFFER pImageBuffer_Temp = g_pcTargetFile->getImageBuffer();
	unsigned char szHex[11] = { "0x" };

	unsigned int functionRvaIndex = 0;
	LV_ITEM lvitem;
	memset(&lvitem, 0, sizeof(LV_ITEM));

	HWND hwndListControl = GetDlgItem(hwndExportDirectoryDirectoryDlg, IDC_LIST_EXPORTDIRECTORY);

	lvitem.mask = LVIF_TEXT;
	unsigned int L = 0;
	ordinal = base + L;

	// Export using ordinal not yet finish!!!!!!!!!!!!!!!
	do {
		ASCIItoHex((unsigned char*)&ordinal, szHex + 2, 2);
		lvitem.pszText = (LPSTR)szHex;
		lvitem.iItem = L;
		lvitem.iSubItem = 0;
		ListView_InsertItem(hwndListControl, &lvitem);
		memset(szHex + 2, 0, 9);

		for (unsigned int i = 1; i <= 3; i++) {
			switch (i) {
			case 1:
				functionRvaIndex = *((unsigned short*)(pImageBuffer_Ordinals + L * 0x02) + 0);
				ASCIItoHex(pImageBuffer_Functions + functionRvaIndex * 0x04, szHex + 2, 4);
				lvitem.pszText = (LPSTR)szHex;
				break;

			case 2:
				functionRvaIndex = *((unsigned short*)(pImageBuffer_Ordinals + L * 0x02) + 0);
				pcTools->RvaToFoa(*((unsigned int*)(pImageBuffer_Functions + functionRvaIndex * 0x04) + 0), &functionFOA, g_pcTargetFile);
				ASCIItoHex((unsigned char*)&functionFOA, szHex + 2, 4);
				lvitem.pszText = (LPSTR)szHex;
				break;

			case 3:
				pcTools->RvaToFoa(*((unsigned int*)(pImageBuffer_Names + L * 0x04) + 0), &nameFOA, g_pcTargetFile);
				lvitem.pszText = (LPSTR)(pImageBuffer_Temp + nameFOA);
				break;
			}
			lvitem.iItem = L;
			lvitem.iSubItem = i;
			ListView_SetItem(hwndListControl, &lvitem);
			memset(szHex + 2, 0, 9);
		}
		L++;
		ordinal++;
	} while (L != numberOfFunctions);
}