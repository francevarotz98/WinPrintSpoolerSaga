# Ethical Hacking project

## Material provided
- [PoC](PoC/)
- [Exe](exe/)
- [Video](video/)


## Environment used
We used VirtualBox with a Windows ISO version 19H1, downloaded through Rufus, from this link:
[Link_to_ISO](https://software.download.prss.microsoft.com/dbazure/Win10_1903_V1_EnglishInternational_x64.iso?t=a7b8800e-5b38-4072-b7b6-aa5e31c4e718&e=1655976965&h=61e61f133be9fc7aada697d56d2afffd87a5abec92cb48fe20758be55a3dd4c6).
The Windows ISO in question is not patched, infact it respects all the requirements for the possible exploitation of the vulnerabilities.
- The name of the user that we gave is: **User**, it does not have privileges.

## Project structure  
The prject is composed by the following files: 
- PoC.cpp: contains the main methods of the application and the init functions for each of the attack
- exploits.h / exploits.cpp: contains the exploits for each vulnerability considered
- utils.h / utils.cpp: contains utils functions, such as: request the reboot of the system, register the current application to be executed after a reboot and check if a directory exists 
- printers.h / printers.cpp: contains utility function to interact with printers and manage printer ports and drivers 

## Steps to run PoC
### Make static .exe file 
In order to create the executable files of the different PoCs, it is needed to statically compile the source code with VisualStudio (have a look to this link for setting the configurations on VisualStudio: [Link_to_static_compilation](https://stackoverflow.com/questions/37398/how-do-i-make-a-fully-statically-linked-exe-with-visual-studio-express-2005).

### Compile attack 
Depending on the attack that we want to perform its needed to: 
1. inside the main function located in PoC.cpp, uncomment the line associated to the init function of the vulnerability considered; 
2. in the case of CVE 2020-1030 and SpoolFool it is also necessary to create a file "payload.txt" inside "C:\Users\User\Desktop". The payload.txt will contain the data that will be inserted inside the file that will be created in "C:\Windows\System32\spool\drivers\x64\4";
3. The executable file will be created inside the x64 folder by Visual Studio.
