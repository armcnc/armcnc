# ARMCNC PACKAGE

⚡ Develop and compile the related drivers and SDK ⚡

## Instructions

> This includes the drivers and SDKs required for the operation and daily use of armcnc and linuxcnc. You can customize and develop them further based on your needs. After completing the development, simply upload to Ubuntu, and all changes will be automatically handled when generating the deb package. You don’t need to worry about these adjustments.

### ethercat_master

This is the EtherCAT Master suitable for the RDK X5 kernel.

```shell
wget https://gitlab.com/etherlab.org/ethercat/-/archive/1.6.2/ethercat-1.6.2.tar.gz
cd ethercat-1.6.2
```

```shell
./bootstrap
./configure --prefix=/usr/local --disable-8139too --enable-generic --with-linux-dir=/usr/src/linux-headers-$(uname -r)
```

```shell
make && make modules
```

```shell
mkdir -p /tmp/ethercat-package
```

```shell
make install DESTDIR=/tmp/ethercat-package
make modules_install INSTALL_MOD_PATH=/tmp/ethercat-package
```

```shell
find /tmp/ethercat-package
```

```shell
sudo rm -rf /tmp/ethercat-package/lib/modules/6.1.83/modules.*
```

```shell
mkdir /tmp/ethercat-package/DEBIAN
```

```shell
vim /tmp/ethercat-package/DEBIAN/control
# input
Package: ethercat-master
Version: 1.6.2-1
Section: utils
Priority: optional
Architecture: arm64
Maintainer: your@email.com
Depends:
Description: EtherCAT Master 1.6.2 EtherCAT master implementation version 1.6.2.
```

```shell
sudo chown -R root:root /tmp/ethercat-package
sudo chmod -R u+rw,go+r-w /tmp/ethercat-package
```

```shell
cd /tmp
dpkg-deb --build ethercat-package ethercat-master_1.6.2-1_arm64.deb
```

```shell
dpkg -c ethercat-master_1.6.2-1_arm64.deb
```

```shell
sudo dpkg -i ethercat-master_1.6.2-1_arm64.deb
```

### orangepi_h3_gpio

This is the driver adapted for the Orangepi H3's 40 GPIO pins, used to control the machine's hardware modules.

```shell
# Compile and Build
sudo halcompile --install orangepi_h3_gpio.c | grep Linking
```

```shell
# Example usage
loadrt [KINS]KINEMATICS
loadrt [EMCMOT]EMCMOT base_period_nsec=[EMCMOT]BASE_PERIOD servo_period_nsec=[EMCMOT]SERVO_PERIOD num_joints=[KINS]JOINTS
loadrt orangepi_h3_gpio pwm_types="p,p,p,p,p,f" in_pins="19,20,21" out_pins="4,5,6"
```

### python_sdk

This is the Python SDK required for armcnc operation, which is used in the startup program of each machine.

1、Install

```shell
pip3 install armcnc websocket-client==0.48.0 requests pyserial colorlog
```

2、Publish

```shell
pip3 install twine setuptools wheel
```

```shell
python3 setup.py sdist bdist_wheel
```

```shell
twine upload dist/*
```

### rdk_x5_gpio

This is the driver adapted for the RDK X5's 40 GPIO pins, used to control the machine's hardware modules.

```shell
# Compile and Build
sudo halcompile --install rdk_x5_gpio.c | grep Linking
```

```shell
# Example usage
loadrt [KINS]KINEMATICS
loadrt [EMCMOT]EMCMOT base_period_nsec=[EMCMOT]BASE_PERIOD servo_period_nsec=[EMCMOT]SERVO_PERIOD num_joints=[KINS]JOINTS
loadrt rdk_x5_gpio pwm_types="p,p,p,p,p,f" in_pins="19,20,21" out_pins="4,5,6"
```