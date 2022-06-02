
#include <string>
#include <iostream>
#include <fstream>
#include <Windows.h>

#include "printer.h"

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
*/
void cve_2020_1030() {
	wchar_t* printerName = const_cast<wchar_t*>(L"testPrinterName");
	HANDLE printerHandler = OpenPrinter(printerName);




	ClosePrinter(printerHandler); 
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

	// redirect cout to output.txt 
	std::ofstream file;
	file.open("output.txt");
	std::streambuf* sbuf = std::cout.rdbuf();
	cout.rdbuf(file.rdbuf());

	/* 
	wchar_t* printerName = const_cast<wchar_t*>(L"testPrinterName");
	wchar_t* driverName = const_cast<wchar_t*>(L"Generic / Text Only");
	wchar_t* portName = const_cast<wchar_t*>(L"c:\\windows\\tracing\\myport.txt");

	// printer settings 
	// HRESULT driverResult = AddDriver(driverName); 
	// DWORD portResult = AddPort(portName); 

	// HANDLE printerHandler = AddPrinter(printerName, driverName, portName); 
	HANDLE printerHandler = OpenPrinter(printerName);
	PrintText(printerHandler, "123prova");

	// close printer 
	ClosePrinter(printerHandler);
	return 0;*/
}
