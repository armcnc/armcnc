#!/usr/bin/env python
# -*- coding: utf-8 -*-

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

import os
import sys
sys.path.append("/usr/lib/python3/dist-packages")
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "..")))
import armcnc

def armcnc_start(sdk):
    sdk.utils.log.ignore("armcnc_start")
    while True:
        pass

def armcnc_message(sdk, message):
    sdk.utils.log.ignore("armcnc_message")

def armcnc_exit(sdk):
    sdk.utils.log.ignore("armcnc_exit")

if __name__ == "__main__":
    armcnc.Framework()
