#!/bin/sh

# Copyright 2024 ARMCNC, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set -e

uname -a && lsb_release -a

architecture=$(dpkg --print-architecture)

version=$(grep 'Version:.*"' "../backend/package/version/version.go" | awk -F'"' '{print $2}')

datetime=$(date +%Y%m%d%H%M%S)

if [ ! -d "debian" ]; then
    mkdir -p debian
    sudo cp -r ubuntu/* ./debian/
    sudo chmod +x debian/DEBIAN/*
    find ./debian -type f -name ".gitkeep" -exec rm -f {} +
else
    sudo rm -rf debian && sudo rm -rf ./*.deb && mkdir -p debian
    sudo cp -r ubuntu/* ./debian/
    sudo chmod +x debian/DEBIAN/*
    find ./debian -type f -name ".gitkeep" -exec rm -f {} +
fi

if [ ! -f "/usr/lib/linuxcnc/modules/rdk_x5_gpio.so" ]; then
    cd ../package/rdk_x5_gpio
    sudo halcompile --install rdk_x5_gpio.c | grep Linking
    cp /usr/lib/linuxcnc/modules/rdk_x5_gpio.so ../../tools/debian/usr/lib/linuxcnc/modules/
    cd ../tools
else
    cp /usr/lib/linuxcnc/modules/rdk_x5_gpio.so ./debian/usr/lib/linuxcnc/modules/
fi

sudo chmod +x ./debian/etc/update-motd.d/*
sudo chmod +x ./debian/home/root/* && sudo find ./debian/home/* -type f -name "*.desktop" -exec chmod +x {} +

/usr/local/go/bin/go env -w GOSUMDB=off
/usr/local/go/bin/go env -w GOPATH=/tmp/golang
/usr/local/go/bin/go env -w GOMODCACHE=/tmp/golang/pkg/mod
export GO111MODULE=on && export GOPROXY=https://goproxy.io
cd ../backend/cmd && /usr/local/go/bin/go mod tidy && /usr/local/go/bin/go build -o ../release/main main.go
sudo cp ../release/main ../../tools/debian/opt/armcnc/backend/release/
sudo cp ../release/config.sample.yaml ../../tools/debian/opt/armcnc/backend/release/
sudo rm -rf ../release/main && cd ../../tools

sudo cp -r ../frontend/release/* ./debian/opt/armcnc/frontend/release/

sudo touch debian/DEBIAN/conffiles && sudo chmod +x debian/DEBIAN/conffiles

sudo touch debian/DEBIAN/control && sudo chmod +x debian/DEBIAN/control
sudo sh -c 'echo "Package: armcnc" >> debian/DEBIAN/control'
version_str="Version: $version-$datetime"
export version_str
sudo -E sh -c 'echo $version_str >> debian/DEBIAN/control'
sudo sh -c 'echo "Maintainer: ARMCNC <admin@geekros.com>" >> debian/DEBIAN/control'
sudo sh -c 'echo "Homepage: https://www.armcnc.net" >> debian/DEBIAN/control'
architecture_str="Architecture: $architecture"
export architecture_str
sudo -E sh -c 'echo $architecture_str >> debian/DEBIAN/control'
sudo sh -c 'echo "Installed-Size: 1048576" >> debian/DEBIAN/control'
sudo sh -c 'echo "Section: utils" >> debian/DEBIAN/control'
sudo sh -c 'echo "Depends:" >> debian/DEBIAN/control'
sudo sh -c 'echo "Recommends:" >> debian/DEBIAN/control'
sudo sh -c 'echo "Suggests:" >> debian/DEBIAN/control'
sudo sh -c 'echo "Description: CNC system for ARM platform" >> debian/DEBIAN/control'

sudo dpkg --build debian && dpkg-name debian.deb

echo "sudo scp armcnc_*.deb root@ip:/data/wwwroot/mirrors.com/ubuntu/pool/main/jammy/ && sudo rm -rf *.deb && sudo rm -rf debian"