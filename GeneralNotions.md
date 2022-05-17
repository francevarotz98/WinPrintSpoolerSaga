# Graphics Device Interface (GDI)
- The Microsoft Windows graphics device interface (GDI) enables applications to use graphics and formatted text on both the video display and the printer. Windows-based applications do not access the graphics hardware directly. Instead, GDI interacts with device drivers on behalf of applications.
  The Graphics Device Interface (GDI) includes both user-mode and kernel-mode components for graphics support.
  
## Links:
- https://docs.microsoft.com/en-us/windows/win32/gdi/windows-gdi
- https://research.ifcr.dk/spoolfool-windows-print-spooler-privilege-escalation-cve-2022-22718-bf7752b68d81

# winspool.drv
- winspool.drv is the client interface into the Spooler. It exports the functions that make up the Spooler’s Win32 API, and provides RPC stubs for accessing the server.

## Links
- https://research.ifcr.dk/spoolfool-windows-print-spooler-privilege-escalation-cve-2022-22718-bf7752b68d81

# spoolsv.exe
- spoolsv.exe is the Spooler’s API server. It is implemented as a service that is started when the operating system is started. This module exports an RPC interface to the server side of the Spooler’s Win32 API. Clients of spoolsv.exe include winspool.drv (locally) and win32spl.dll (remotely). The module implements some API functions, but most function calls are passed to a print provider by means of the router (spoolss.dll).

## Links
- https://research.ifcr.dk/spoolfool-windows-print-spooler-privilege-escalation-cve-2022-22718-bf7752b68d81
   
# Registry Hives
- A hive is a logical group of keys, subkeys, and values in the registry that has a set of supporting files loaded into memory when the operating system is started or a user logs in.

## Links
- https://docs.microsoft.com/en-us/windows/win32/sysinfo/registry-hives


# HKEY_LOCAL_MACHINE (HKLM)
- It is one of several registry hives that make up the Windows Registry. This particular hive contains the majority of the configuration information for the software you have installed, as well as for the Windows operating system itself.
  In addition to software configuration data, this hive also contains lots of valuable information about currently detected hardware and device drivers.
  
## Links
- https://www.lifewire.com/hkey-local-machine-2625902

# .SPL
- The SPL file is nothing more than a copy, essentially, of all the data that is meant to go the printer.
  An SPL file is a file created by Windows when a user submits a print job. It contains the spooled print data, which includes the digital document contents sent to the printer for printing. SPL files save data in a raw printer format and are used as a temporary data store for the print job, which is emptied when the job completes.

## Links
- https://windows-internals.com/printdemon-cve-2020-1048/
- https://fileinfo.com/extension/spl

# .SHD
- File created by Windows when a user submits a print job; stores header information for a print job, but does not store the actual spooled document data; allows the print job settings to be "shadowed" so that the job can be resubmitted again if there is an interruption.

## Links
- https://fileinfo.com/extension/shd

# EDR
- Microsoft Defender for Endpoint is a holistic, cloud-delivered, endpoint security solution. 

## Links
- https://docs.microsoft.com/en-us/azure/defender-for-cloud/integration-defender-for-endpoint?tabs=linux

# InterProcess Communications (IPC)
- The Windows operating system provides mechanisms for facilitating communications and data sharing between applications. Collectively, the activities enabled by these mechanisms are called interprocess communications (IPC). Some forms of IPC facilitate the division of labor among several specialized processes. Other forms of IPC facilitate the division of labor among computers on a network.

## Links
- https://docs.microsoft.com/en-us/windows/win32/ipc/interprocess-communications

# Impersonation
- Impersonation is the ability of a thread to execute in a security context that is different from the context of the process that owns the thread. When running in the client's security context, the server "is" the client, to some degree. The server thread uses an access token representing the client's credentials to obtain access to the objects to which the client has access.

# Links
- https://docs.microsoft.com/en-us/windows/win32/ipc/interprocess-communications

# ReadPrinter 
- The ReadPrinter function retrieves data from the specified printer.

## Links
- https://docs.microsoft.com/en-us/windows/win32/printdocs/readprinter

# WritePrinter
- The WritePrinter function notifies the print spooler that data should be written to the specified printer.

## Links
- https://docs.microsoft.com/en-us/windows/win32/printdocs/writeprinter

# StartDocPrinter
- The StartDocPrinter function notifies the print spooler that a document is to be spooled for printing.

## Links
- https://docs.microsoft.com/en-us/windows/win32/printdocs/startdocprinter

# SetJobs
- The SetJob function pauses, resumes, cancels, or restarts a print job on a specified printer. You can also use the SetJob function to set print job parameters, such as the print job priority and the document name.

## Links
- https://docs.microsoft.com/en-us/windows/win32/printdocs/setjob

# OpenPrinter
- The OpenPrinter function retrieves a handle to the specified printer or print server or other types of handles in the print subsystem.

## Links
- https://docs.microsoft.com/en-us/windows/win32/printdocs/openprinter

# EnumJobs
- The EnumJobs function retrieves information about a specified set of print jobs for a specified printer.

## Links
- https://docs.microsoft.com/en-us/windows/win32/printdocs/enumjobs

# WriteFile
- Writes data to the specified file or input/output (I/O) device.

## Links
- https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-writefile

# ScheduleJob
- The ScheduleJob function requests that the print spooler schedule a specified print job for printing.

## Links
- https://docs.microsoft.com/en-us/windows/win32/printdocs/schedulejob


