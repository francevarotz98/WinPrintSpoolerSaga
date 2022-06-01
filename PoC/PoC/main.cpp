
#include <string>
#include <iostream>
#include <fstream>
#include <Windows.h>

#include "printer.h"

using std::cout;
using std::endl;
using std::string;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

	// redirect cout to output.txt 
	std::ofstream file;
	file.open("output.txt");
	std::streambuf* sbuf = std::cout.rdbuf();
	cout.rdbuf(file.rdbuf());

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
	return 0;
}
