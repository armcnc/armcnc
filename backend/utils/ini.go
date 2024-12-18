// Copyright 2024 ARMCNC, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package utils

import "gopkg.in/ini.v1"

func IniEmpty() *ini.File {
	iniFile := ini.Empty()
	return iniFile
}

func IniReflectFrom(cfg *ini.File, v interface{}) error {
	err := ini.ReflectFrom(cfg, v)
	return err
}

func IniLoad(source interface{}) (*ini.File, error) {
	iniFile, err := ini.Load(source)
	return iniFile, err
}

func IniSaveTo(cfg *ini.File, filename string) error {
	err := cfg.SaveTo(filename)
	return err
}

func IniMapTo(cfg *ini.File, v interface{}) error {
	err := cfg.MapTo(v)
	return err
}
