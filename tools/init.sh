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

sudo apt update -y && sudo sudo apt upgrade -y

sudo apt install -y dpkg-dev gpg

if [ ! -d "/usr/local/go/bin/" ]; then
    golang="1.22.0"
    sudo wget -q https://studygolang.com/dl/golang/go"${golang}".linux-"${architecture}".tar.gz && sudo tar -C /usr/local -xzf go"${golang}".linux-"${architecture}".tar.gz
    sudo sh -c 'echo "export PATH=$PATH:/usr/local/go/bin" >> /etc/profile'
    # shellcheck disable=SC2039
    # shellcheck disable=SC1090
    source /etc/profile && source ~/.bashrc
    sudo rm -rf go"${golang}".linux-"${architecture}".tar.gz
fi