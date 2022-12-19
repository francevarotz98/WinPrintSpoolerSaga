# Ethical Hacking project
Bash and C++ PoCs for critical / high impact Windows Spooler vulnerabilities found in 2020-22 years: 
- [CVE-2020-1337](https://nvd.nist.gov/vuln/detail/CVE-2020-1037) 
- [CVE-2020-1048, aka PrintDemon](https://nvd.nist.gov/vuln/detail/cve-2020-1048)
- [CVE-2020-1030](https://nvd.nist.gov/vuln/detail/CVE-2020-1030)
- [CVE-2022-21999, aka SpoolFool](https://nvd.nist.gov/vuln/detail/CVE-2022-21999)

The videos of the demonstration of the PoCs are provided inside the [Video](video/) folder, while more information about each specific vulnerability can be found in the [CVEs history](CVEs.md) file. 

## Environment used
We used VirtualBox with a Windows ISO version 19H1, downloaded through Rufus, from this link:
[Link to ISO](https://software.download.prss.microsoft.com/dbazure/Win10_1903_V1_EnglishInternational_x64.iso?t=a7b8800e-5b38-4072-b7b6-aa5e31c4e718&e=1655976965&h=61e61f133be9fc7aada697d56d2afffd87a5abec92cb48fe20758be55a3dd4c6).

The link is working on the 22/06/2022, otherwise inside Rufus, follow the follwing steps:
1. Click the arrow on the button named "Select" and select "Download";
2. A new window will pop up, select the version Windows 10 and press the button "Next";
3. On release, select 19H1 one and then press the button "Next";
4. Leave the default edition of Windows (i.e., Home/Pro) and press "Next";
5. Choose the lenguage and architecture and press "Next";
6. If you want to download via browser tick the square and then press "Download".

The Windows ISO in question is not patched, in fact it respects all the requirements for the possible exploitation of the vulnerabilities.
- The name of the user that we gave is: **User**, it does not have privileges.

## Project structure  
The prject is composed by the following files: 
- **PoC.cpp**: contains the main methods of the application and the init functions for each of the attack;
- **exploits.h / exploits.cpp**: contains the exploits for each vulnerability considered;
- **utils.h / utils.cpp**: contains utils functions, such as: request the reboot of the system, register the current application to be executed after a reboot and check if a directory exists ;
- **printers.h / printers.cpp**: contains utility function to interact with printers and manage printer ports and drivers.

## Steps to run PoC
### Make static .exe file 
In order to create the executable files of the different PoCs, it is needed to statically compile the source code with VisualStudio (have a look to this link for setting the configurations on VisualStudio: [Link to static compilation](https://stackoverflow.com/questions/37398/how-do-i-make-a-fully-statically-linked-exe-with-visual-studio-express-2005).

### Compile attack 
Depending on the attack that we want to perform its needed to: 
1. inside the main function located in PoC.cpp, uncomment the line associated to the init function of the vulnerability considered; 
2. in the case of CVE 2020-1030 and SpoolFool it is also necessary to create a file "payload.txt" inside "C:\Users\User\Desktop". The payload.txt will contain the data that will be inserted inside the file that will be created in "C:\Windows\System32\spool\drivers\x64\4";
3. The executable file will be created inside the x64 folder by Visual Studio.

## Beyond C++ - Powershell
Beyond using the C++ API, we also wanted to try the exploitation of PrintDemon using Powershell.
Inside the directory [PoC PrintDemon Powershell](<PrintDemon PowerShell/>) there is the implementation of a version of PrintDemon which can be run via Powershell.
