# CVE list

## Print Demon
It allows an attacker to write an arbitrary file by creating a printer port that pointed to a file on a disk.

- CVE-2020-1048


## SpoolFool

- CVE-2020-1337
- CVE-2020-1030



## Print Nightmare
It allows remote code execution (RCE)

<!--- CVE-2021-1675 addresses a different vulnerability in the same Print Spooler API call --->

- CVE-2021-34527

### Windows versions for Print Nightmare

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
