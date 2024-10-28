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
import shutil
from setuptools import setup, find_packages

dist_directory = "dist"
if os.path.exists(dist_directory):
    shutil.rmtree(dist_directory)
    shutil.rmtree("build")
    shutil.rmtree("armcnc.egg-info")

with open("readme.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

setup(
    name="armcnc",
    version="1.1.26",
    author="MakerYang",
    author_email="admin@wileho.com",
    description="Python development framework for armcnc.",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/armcnc/python",
    packages=find_packages(),
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
    python_requires=">=3.7.5",
)