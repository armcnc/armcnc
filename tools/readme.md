# ARMCNC TOOLS

⚡ Build the required armcnc deb package for Ubuntu ⚡

## Compile and generate the deb file

1、Set up and install the build environment

```shell
cd tools
sudo ./init.sh
```

2、Generate the deb installation file

```shell
sudo ./publish.sh
```

## Important Notes

1、Please perform the related operations on Ubuntu 22.04, either on the x86 or ARM64 platform.

2、The backend, frontend, and package modules can be customized and further developed as needed, then compiled and packaged according to the above instructions.

3、The packaged deb file can be directly installed using the following command or uploaded to the cloud.

```shell
sudo dpkg -i armcnc*.deb
```