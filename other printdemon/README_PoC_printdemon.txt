+ compile cs file to DLL : 
   C:\Windows\Microsoft.NET\Framework\v4.0.30319\csc.exe /target:library <nameFile.cs>

+ use DLL file in powershell:
   Add-Type -Path <path/to/dll>

+ to run a specific function:
   [<namespace>.<class>]::function([parameter(s)])
   