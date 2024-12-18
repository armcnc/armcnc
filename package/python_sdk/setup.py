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

from setuptools import setup, find_packages

setup(
    name="armcnc",
    version="2.1.7",
    author="MakerYang",
    author_email="admin@wileho.com",
    description="Python development framework for armcnc.",
    long_description="",
    long_description_content_type="text/markdown",
    url="https://github.com/armcnc/armcnc",
    packages=find_packages(),
    classifiers=[
        "Intended Audience :: Developers",
        "License :: OSI Approved :: Apache Software License",
        "Topic :: Multimedia :: Sound/Audio",
        "Topic :: Multimedia :: Video",
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3 :: Only",
    ],
    python_requires=">=3.9.0",
    license="Apache-2.0",
    project_urls={
        "Documentation": "https://www.armcnc.net",
        "Website": "https://armcnc.net",
        "Source": "https://github.com/armcnc/armcnc",
    },
    keywords=["armcnc", "cnc", "arm64", "arm", "linuxcnc", "ubuntu"],
    install_requires=["requests>=2.32.3"]
)
