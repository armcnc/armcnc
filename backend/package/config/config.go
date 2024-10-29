// Copyright 2024 GEEKROS, Inc.
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

package config

import (
	"gopkg.in/yaml.v3"
	"os"
	"path/filepath"
	"time"
)

var Get = &Config{}

type Config struct {
	Path      string  `yaml:"-"`
	Workspace string  `yaml:"-"`
	Server    service `yaml:"server"`
	Auth      auth    `yaml:"auth"`
}

type service struct {
	Mode         string        `yaml:"mode"`
	Port         int           `yaml:"port"`
	ReadTimeout  time.Duration `yaml:"read_timeout"`
	WriteTimeout time.Duration `yaml:"write_timeout"`
}

type auth struct {
	Token string `yaml:"token"`
}

func New() *Config {
	workspace := "/opt/armcnc"

	configPath := os.Getenv("CLIENT_CONFIG_PATH")
	if configPath == "" {
		configPath = filepath.Join(workspace, "/backend/release/config.sample.yaml")
	}

	return &Config{
		Path:      configPath,
		Workspace: workspace,
	}
}

func (c *Config) LoadConfig() *Config {
	file, err := os.ReadFile(c.Path)
	if err != nil {
		return c
	}

	err = yaml.Unmarshal(file, c)
	if err != nil {
		return c
	}

	_, err = os.Stat(c.Workspace + "/runtime/auth_token")
	if err != nil {
		if os.IsNotExist(err) {
			return c
		}
		return c
	}

	file, err = os.ReadFile(c.Workspace + "/runtime/auth_token")
	if err != nil {
		return c
	}

	c.Auth.Token = string(file)

	return c
}
