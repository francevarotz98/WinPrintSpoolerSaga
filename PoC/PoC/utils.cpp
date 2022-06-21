
#include <string>
#include <iostream>
#include <Windows.h>

#include "utils.h"; 

using std::cout;
using std::endl;
using std::string;

wchar_t* RESTART_COMMAND = const_cast<wchar_t*>(L"RESTART");

// reboot the system and register the application to be started after the next boot 
bool registerApplicationAndRestartSystem() {
	HRESULT hr = S_OK;

	// register the application 
	hr = RegisterApplicationRestart(RESTART_COMMAND, 0);
	if (FAILED(hr)) {
		cout << "[-] Error while registering the application for restart" << endl;
		return FALSE;
	}
	cout << "[+] Application registered successfully" << endl;

	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;

	// Get a token for the current process 
	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		cout << "[-] Error while obtaining token for current process: " << GetLastError() << endl;
		return FALSE;
	}
	cout << "[+] Obtained token for current process" << endl;

	// Get the LUID for the shutdown privilege. 
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// Get the shutdown privilege for this process. 
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
	if (GetLastError() != ERROR_SUCCESS) {
		cout << "[-] Error trying to obtain shutdown privileges: " << GetLastError() << endl;
		return FALSE;
	}
	cout << "[+] Successflully obtained shutdown privileges" << endl;

	// sleep for at least 1 minute, since windows restart the apps that have been active for at least that amount of time 
	cout << "[i] Sleeping for 62 seconds" << endl;
	Sleep(62 * 1000);

	// restart the system and apps
	DWORD shutdownResult =
		InitiateShutdownA(NULL, NULL, 15, SHUTDOWN_RESTART | SHUTDOWN_RESTARTAPPS, SHTDN_REASON_MAJOR_LEGACY_API);
	if (shutdownResult != ERROR_SUCCESS) {
		cout << "[-] Error while restarting the system: " << shutdownResult << endl;
		return FALSE;
	}

	cout << "[i] System will restart in 15 seconds" << endl;
	return TRUE;
}

// depending on the method the app has started, it is called with different commands and parameter 
// the function returns true if it has been runned after a system reboot, otherwise false 
bool isAppStartedFromReboot(int argc, wchar_t* argv[]) {
	return argc > 1; //  && !wcsncmp(RESTART_COMMAND, argv[1], sizeof(RESTART_COMMAND));
}

// check if the directory having dirPath as path exists 
bool doesDirExists(wchar_t* dirPath) {
	DWORD ftyp = GetFileAttributes(dirPath);
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with the path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;    // this is not a directory!
}