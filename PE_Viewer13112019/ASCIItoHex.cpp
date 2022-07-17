#include "ASCIItoHex.h"

bool ASCIItoHex(IN unsigned char* szString, OUT unsigned char* szBuffer, int size) {
	char szTempBuffer[11] = { 0 };

	for (int i = 0; i < size; i++) {
		sprintf(szTempBuffer, "%02X", szString[i]);
		strncpy((char*)szBuffer + (-1 * (i - (size - 1)) * 2), szTempBuffer, 2);
	}

	return 0;
}