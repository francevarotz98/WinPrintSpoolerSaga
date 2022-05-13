# CVE list

## Print Demon
It allows an attacker to write an arbitrary file by creating a printer port that pointed to a file on a disk.

- CVE-2020-1048

### Resources 
 - https://windows-internals.com/printdemon-cve-2020-1048/

### Windows versions needed for CVE-2020-1048
Windows 10 Version 1803, Windows 10 Version 1809, Windows 10 Version 1709, Windows 10, Windows 10 Version 1607, Windows 10 Version 1909, Windows 10 Version 1903, Windows 7, Windows 8.1, Windows RT 8.1, Windows Server version 1803 (Core Installation),Windows Server 2019 (Core installation), Windows Server 2016 (Core installation), Windows Server 2008 (Core installation), Windows Server 2008 (Core installation), Windows Server 2008 R2 (Core installation), Windows Server 2012 (Core installation), Windows Server 2012 R2.

<!--- "It impacts all Windows versions going back to Windows NT 4, released in 1996." (In diversi siti scrivono quanto scritto in questo commento, altri quanto scritto sopra che più o meno è lo stesso concetto con le varie versioni, e ho anche filtrato un pò.... In generale per le varie versioni di Windows che sono affette dalla CVE mi sono basato sulla pagina ufficiale di microsoft della CVE specifica, in fondo alla pagina li trovate) --->


## SpoolFool

- CVE-2020-1337   That is a bypass of (PrintDemon) CVE-2020-1048’s patch via a junction directory.
- CVE-2020-1030   Allows arbitrary writing to the file system.

### Resources 
https://research.ifcr.dk/spoolfool-windows-print-spooler-privilege-escalation-cve-2022-22718-bf7752b68d81

### Windows versions needed for SpoolFool CVE-2020-1337 

Windows 7, Windows RT 8.1, Windows 10,  Windows server 2008, Windows server 2012 R2, Windows server 2016, Windows server 2019, Windows server 2024.

### Windows versions needed for SpoolFool CVE-2020-1030

Windows 7, Windows 8.1, Windows 8.1 RT, Windows 10, Windows server 2004, Windows server 2008, Windows server 2008 R2, Windows server 2012, Windows server 2012 R2, Windows server 2019.


## Print Nightmare
It allows remote code execution (RCE)

<!--- CVE-2021-1675 addresses a different vulnerability in the same Print Spooler API call. For CVE 1675, the attacker needs to have direct access to the machine to use a malicious DLL file to escalate privileges. While with CVE 34527, the attacker can remotely inject DLLs. --->

- CVE-2021-34527

### Windows versions needed for CVE-2021-34527

Windows Server 2004, Windows Server 2008, Windows Server 2012, Windows Server 2012 R2, Windows Server 2016, Windows Server 2019, Windows 7, Windows RT 8.1, Windows 8.1, and Windows 10.

Microsoft release a list of updates (KBXXXXX) corresponding to each version of Windows concerned.
However Microsoft's PrintNightmare security patch is incomplete.

## Stuxnet

<!--- Secondo me ci sta citarla, senza soffermarsi troppo (magari nel paper), dicendo che alcune di queste vulnerabilità erano già state utilizzate precedentemente per Stuxnet ecc --->

- CVE-2020-1337   (Linked to SpoolFool)
- CVE-2020-1070
- CVE-2020-1048   (Linked to SpoolFool)
- CVE-2019-0683
- CVE-2010-2729 


# Bibliography
- https://www.secplicity.org/2022/02/23/spoolfool-windows-print-spooler-fooled-again/
- https://blog.sygnia.co/demystifying-the-print-nightmare-vulnerability
- https://www.accenture.com/us-en/blogs/cyber-defense/discovering-exploiting-shutting-down-dangerous-windows-print-spooler-vulnerability
- https://voidsec.com/cve-2020-1337-printdemon-is-dead-long-live-printdemon/
