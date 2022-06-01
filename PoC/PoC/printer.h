#pragma once

#include <windows.h>

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

	PRINTER_INFO_2 printerInfo = { 0 };
	printerInfo.pPortName = portName;
	printerInfo.pDriverName = driverName;
	printerInfo.pPrinterName = printerName;
	printerInfo.pPrintProcessor = printProcessor;
	printerInfo.pDatatype = datatype;

	return AddPrinter(NULL, 2, (LPBYTE)&printerInfo);
}

HANDLE OpenPrinter(wchar_t* printerName) {
	HANDLE printerHandler = NULL;
	OpenPrinter(printerName, &printerHandler, NULL);
	return printerHandler;
}

BOOL PrintText(HANDLE printerHandler, string text) {
	DOC_INFO_1 docInfo;
	docInfo.pDatatype = const_cast<wchar_t*>(L"RAW");
	docInfo.pOutputFile = NULL;
	docInfo.pDocName = const_cast<wchar_t*>(L"Document");
	StartDocPrinter(printerHandler, 1, (LPBYTE)&docInfo);

	DWORD dwNeeded = (DWORD)strlen(text.c_str());
	char* pText = const_cast<char*>(text.c_str());
	if (!WritePrinter(printerHandler, pText, dwNeeded, &dwNeeded))
		return FALSE;

	return EndDocPrinter(printerHandler);
}