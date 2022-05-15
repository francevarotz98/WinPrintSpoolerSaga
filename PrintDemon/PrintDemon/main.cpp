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

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	
	// add a driver called "Generic / Text Only", with local computer as print server and inf file shipped with Windows
	HRESULT result = InstallPrinterDriverFromPackage(NULL, NULL, L"Generic / Text Only", NULL, 0);

	// add a port TODO


	return 0;
}
