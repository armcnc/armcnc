# ARMCNC

⚡ CNC system for ARM platform ⚡

## License

[![License:Apache2.0](https://img.shields.io/badge/License-Apache2.0-yellow.svg)](https://opensource.org/licenses/Apache2.0)

## Automated installation and deployment

Running the script below will automatically complete everything for you.

```shell
sudo curl -s https://mirrors.armcnc.net/debian/install.sh|bash
```

## Manual installation and deployment

You can also manually install and deploy through the following steps to better understand every aspect of the process.

1、Retrieve system and environment information

```shell
lsb_release -a
architecture=$(dpkg --print-architecture)
codename=$(lsb_release -c | awk '{print $2}')
```

2、Set up a dedicated software repository for armcnc

```shell
sudo touch /etc/apt/sources.list.d/armcnc.list
sudo wget -q -O /tmp/Release.gpg https://mirrors.geekros.com/ubuntu/dists/jammy/Release.public
sudo gpg --dearmor < /tmp/Release.gpg > /etc/apt/trusted.gpg.d/armcnc-mirrors.gpg
echo "deb [arch=${architecture},all] https://mirrors.geekros.com/ubuntu ${codename} main" | sudo tee /etc/apt/sources.list.d/armcnc.list
```

3、Update

```shell
sudo apt -y update
```

4、Install basic dependencies

```shell
sudo apt install -y openssl nginx chromium-browser
```

5、Install linuxcnc and the necessary dependencies

```shell
sudo apt install -y libespeak1 libespeak-ng1 libpcaudio0 libsonic0 python3-pyqt5.sip libqscintilla2-qt5-15 libgtksourceview-3.0-1 gir1.2-gtksource-3.0 mesa-utils-bin
sudo apt install -y mesa-utils python3-opengl python3-configobj libgtksourceview-3.0-dev tclreadline python3-pyqt5 python3-pyqt5.qsci python3-pyqt5.qtsvg python3-pyqt5.qtopengl python3-espeak python3-dbus.mainloop.pyqt5 python3-pyqt5.qtwebengine espeak-ng espeak pyqt5-dev-tools gstreamer1.0-tools python3-poppler-qt5
sudo apt install -y linuxcnc-uspace linuxcnc-uspace-dev
```

6、Install and configure EtherCAT

```shell
sudo apt install -y ethercat-master libethercat-dev linuxcnc-ethercat
MAC_ADDRESS=$(cat /etc/network/mac_address)
sed -i "s/^MASTER0_DEVICE=\".*\"/MASTER0_DEVICE=\"$MAC_ADDRESS\"/" /usr/local/etc/ethercat.conf
sed -i "s/^DEVICE_MODULES=\".*\"/DEVICE_MODULES=\"generic\"/" /usr/local/etc/ethercat.conf
sudo touch /etc/udev/rules.d/99-ethercat.rules
cat <<-EOF > /etc/udev/rules.d/99-ethercat.rules
KERNEL=="EtherCAT[0-9]", MODE="0777"
EOF
sudo ldconfig
sudo depmod -a
sudo systemctl enable ethercat.service
sudo systemctl restart ethercat.service
```

7、Install armcnc

```shell
sudo apt install -y armcnc
```

## Thank

[D-Robotics](https://github.com/d-robotics)

[Linuxcnc](https://github.com/LinuxCNC/linuxcnc)

[EtherCAT](https://download.opensuse.org/repositories/science:/EtherLab/Debian_12/arm64/)
[readme.md](readme.md)
[Ubuntu](https://github.com/ubuntu)