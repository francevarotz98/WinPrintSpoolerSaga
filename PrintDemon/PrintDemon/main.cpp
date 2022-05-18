/*
	A printer has associated a minimum of two elements:
	- a port e.g. USB port, TCP/UP port (and address) or even FILE: (which means that printer can print to a file, PORTPROMPT: on windows 8 and above)
	- a driver

	Spooler service is implemented in Spools.exe, which runs with SYSTEM privileges

	To create a new printer we need to install a driver to associate to it, and from Windows Vista if the driver is a pre-existing, inbox-driver no privileges are needed

	Example with a Generic / Text-Only driver:
	 - by powershell: Add-PrinterDriver -Name "Generic / TextOnly"
	 - with code: hr = InstallPrinterDriverFromPackage(NULL, NULL, L"Generic / Text Only", NULL, 0);

	 Note: LPCTSTR = pointer to a constant, null-terminated string
	 InstallPrinterDriverFromPackage requires:
	  - pszServer: LPCTSTR, name of the print server, if NULL is passed => local computer
	  - pszInfPath: LPCTSTR, path to the driver's inf file, if NULL => the .inf file is shipped with Windows
	  - pszDriverName: LPCTSTR, custom name for the driver
	  - pszEnvironment, LPCTSTR, processor architecutre, can be NULL
	  - dwFlags: to better understand

	InstallPrinterDriverFromPackage returns HRESULT (a result code): S_OK if everything is fine, otherwise a value representing the specific error

	To list all drivers execute "Get-PrinterDriver" in Powershell
*/

#include <windows.h>
#include <string>
#include <iostream>
#include <fstream>

using std::cout; 
using std::endl;
using std::string;

// add a driver called "Generic / Text Only", with local computer as print server and inf file shipped with Windows
HRESULT AddDriver(wchar_t* driverName) {
	return InstallPrinterDriverFromPackage(NULL, NULL, driverName, NULL, 0);
}

// add a port with portName name
DWORD AddPort(wchar_t* portName) {
	HANDLE hPrinter; // printer handler 
	PRINTER_DEFAULTS PrinterDefaults;
	memset(&PrinterDefaults, 0, sizeof(PrinterDefaults)); // clear PRINTER_DEFAULTS structure 

	PrinterDefaults.pDatatype = NULL;
	PrinterDefaults.pDevMode = NULL;
	PrinterDefaults.DesiredAccess = SERVER_ACCESS_ADMINISTER; // note: we are requesting the privileges to create a new port (all users can) 

	// try to open a handle to the XCV port of the local spooler 
	string xcvLocalPortStr = ", XcvMonitor Local Port";
	wchar_t* xcvLocalPort = const_cast<wchar_t*>(L", XcvMonitor Local Port");
	if (!OpenPrinter(xcvLocalPort, &hPrinter, &PrinterDefaults)) {
		cout << "Error while trying to open XCV port of the local spooler" << endl;
		return -1;
	}

	DWORD error = 0;

	// XcvData allows to call a function on the spooling server, in this case we call AddPort 
	DWORD portNameSize = ((DWORD)wcslen(portName) + 1) * sizeof(WCHAR);; // size of the string portName in byte
	DWORD needed, xcvresult;
	if (!XcvData(hPrinter, L"AddPort", (LPBYTE)portName, portNameSize, NULL, 0, &needed, &xcvresult)) {
		error = GetLastError();
		cout << "Error in XcvData" << endl;
		cout << "Error: " << error << " (: " << std::hex << error << ")" << endl;
	}

	// try to close the handler to the printer 
	if (!ClosePrinter(hPrinter)) {
		error = GetLastError();
		cout << "Error in closing the printer" << endl;
		cout << "Error: " << error << " (: " << std::hex << error << ")" << endl;
	}

	return error; 
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	
	// redirect cout to output.txt 
	std::ofstream file;
	file.open("output.txt");
	std::streambuf* sbuf = std::cout.rdbuf();
	std::cout.rdbuf(file.rdbuf());

	wchar_t* printerName = const_cast<wchar_t*>(L"testPrinterName");
	wchar_t* driverName = const_cast<wchar_t*>(L"Generic / Text Only");
	wchar_t* portName = const_cast<wchar_t*>(L"c:\\windows\\tracing\\myport.txt");

	// printer settings 
	HRESULT driverResult = AddDriver(driverName); 
	DWORD portResult = AddPort(portName); 


	return 0;
}
