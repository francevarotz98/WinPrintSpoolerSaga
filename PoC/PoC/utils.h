#ifndef UTILS_H
#define UTILS_H

extern wchar_t* RESTART_COMMAND; 
bool registerApplicationAndRestartSystem(); 
bool isAppStartedFromReboot(int argc, wchar_t* argv[]); 
bool doesDirExists(wchar_t* dirPath); 

#endif