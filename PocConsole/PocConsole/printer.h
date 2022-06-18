#pragma once

#include <Windows.h>
#include <string>

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
		// cout << "Error while trying to open XCV port of the local spooler" << endl;
		return -1;
	}

	DWORD error = 0;

	// XcvData allows to call a function on the spooling server, in this case we call AddPort 
	DWORD portNameSize = ((DWORD)wcslen(portName) + 1) * sizeof(WCHAR);; // size of the string portName in byte
	DWORD needed, xcvresult;
	if (!XcvData(hPrinter, L"AddPort", (LPBYTE)portName, portNameSize, NULL, 0, &needed, &xcvresult)) {
		error = GetLastError();
		// cout << "Error in XcvData" << endl;
		// cout << "Error: " << error << " (: " << std::hex << error << ")" << endl;
	}

	// try to close the handler to the printer 
	if (!ClosePrinter(hPrinter)) {
		error = GetLastError();
		// cout << "Error in closing the printer" << endl;
		// cout << "Error: " << error << " (: " << std::hex << error << ")" << endl;
	}

	return error;
}

HANDLE AddPrinter(wchar_t* printerName, wchar_t* driverName, wchar_t* portName) {

	// Windows Server comes with built-in print processors. The one installed by default is known as WinPrint, and it handles the standard data types printed by Windows applications.
	wchar_t* printProcessor = const_cast<wchar_t*>(L"WinPrint");
	wchar_t* datatype = const_cast<wchar_t*>(L"RAW");
	
	PRINTER_INFO_2 printerInfo = {0};

	memset(&printerInfo, 0, sizeof(printerInfo));
	printerInfo.pPrinterName = printerName;
	printerInfo.pDriverName = driverName; 
	printerInfo.pPortName = portName; 
	printerInfo.pPrintProcessor = printProcessor; 
	printerInfo.pDatatype = datatype; 
	printerInfo.Attributes = PRINTER_ATTRIBUTE_HIDDEN;

	return AddPrinter(NULL, 2, (LPBYTE)&printerInfo);
}

HANDLE OpenPrinter(wchar_t* printerName) {

	PRINTER_DEFAULTS printerDefaults;
	memset(&printerDefaults, 0, sizeof(printerDefaults)); // clear PRINTER_DEFAULTS structure 

	printerDefaults.pDatatype = NULL;
	printerDefaults.pDevMode = NULL;
	printerDefaults.DesiredAccess = PRINTER_ALL_ACCESS;

	HANDLE printerHandler = NULL;
	OpenPrinter(printerName, &printerHandler, &printerDefaults);
	return printerHandler;
}
