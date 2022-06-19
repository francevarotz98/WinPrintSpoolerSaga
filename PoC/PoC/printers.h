#ifndef PRINTERS_H
#define PRINTERS_H

#include <Windows.h>

HRESULT AddDriver(wchar_t* driverName); 
DWORD AddPort(wchar_t* portName); 
HANDLE AddPrinter(wchar_t* printerName, wchar_t* driverName, wchar_t* portName); 
HANDLE OpenPrinter(wchar_t* printerName); 

#endif