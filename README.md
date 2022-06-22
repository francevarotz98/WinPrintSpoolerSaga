# Ethical Hacking project

## Material provided
- Report
- Source code
- PoC
- Videos

## Environment used
We used a virtual box with the Windows iso 19H1 (Check if correct), downloaded through Rufus, from this link: (Link da aggiungere, quello su discord non è più utilizzabile).
The Windows iso in question is not patched, infact it respect all the requirements for the possible exploitation of the vulnerabilities starting from PrintDemon (First vulnerability analyzed).
-> user used named: User 

## Project structure  
The prject is composed by the following file: 
- PoC.cpp: contains the main method of the application and the init functions for each of the attack
- exploits.h / exploits.cpp: contains the exploits for each vulnerability considered
- utils.h / utils.cpp: contains utils functions, such as: request the reboot of the system, register the current application to be executed after a reboot and check if a directory exists 
- printers.h / printers.cpp: contains utility function to interact with printers and manage printer ports and drivers 

## Steps to run PoC
### Make static .exe file 
Vedi: https://stackoverflow.com/questions/37398/how-do-i-make-a-fully-statically-linked-exe-with-visual-studio-express-2005

### Compile attack 
Depending on the attack that we want to perform its needed to: 
1. inside the main function located in PoC.cpp, uncomment the line associated to the init function of the vulnerability considered 
2. in the case of cve_2020_1030 and spool_fool it is also necessary to create a file "payload.txt" inside "C:\Users\User\Desktop". The payload.txt will contain the data that will be inserted inside the file that will be create "C:\Windows\System32\spool\drivers\x64\4" 
3. The executable file will be created inside the x64 folder by Visual Studio
