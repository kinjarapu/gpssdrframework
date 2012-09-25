sudo addgroup gn3s
sudo usermod -G gn3s -a $USERNAME
echo 'ACTION=="add", SUBSYSTEM=="usb", ATTR{idVendor}=="1781", ATTR{idProduct}=="0b39", GROUP:="gn3s", MODE:="0777"' > tmpfile
echo 'ACTION=="add", SUBSYSTEM=="usb", ATTR{idVendor}=="1781", ATTR{idProduct}=="0b38", GROUP:="gn3s", MODE:="0777"' >> tmpfile
echo 'ACTION=="add", SUBSYSTEM=="usb", ATTR{idVendor}=="16c0", ATTR{idProduct}=="072f", GROUP:="gn3s", MODE:="0777"' >> tmpfile
sudo chown root.root tmpfile
sudo mv tmpfile /etc/udev/rules.d/10-gn3s.rules
sudo service udev restart
ls -lR /dev/bus/usb | grep gn3s
