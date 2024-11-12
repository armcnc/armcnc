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

class Config:

    def __init__(self):
        self.workspace = "/opt/armcnc"
        self.runtime = f"{self.workspace}/runtime"
        self.path = ""

    def get_workspace(self):
        return self.workspace

    def get_runtime(self):
        return self.runtime

    def get_path(self):
        return self.path

    def set_path(self, path):
        self.path = path