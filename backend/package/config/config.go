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

package config

import (
	"gopkg.in/yaml.v3"
	"os"
	"path/filepath"
	"time"
)

var Get = &Config{}

type Config struct {
	Workspace string  `yaml:"-"`
	Runtime   string  `yaml:"-"`
	Server    service `yaml:"server"`
	Auth      auth    `yaml:"auth"`
	Machine   machine `yaml:"machine"`
}

type service struct {
	Mode         string        `yaml:"mode"`
	Port         int           `yaml:"port"`
	ReadTimeout  time.Duration `yaml:"read_timeout"`
	WriteTimeout time.Duration `yaml:"write_timeout"`
}

type auth struct {
	Getaway string `yaml:"getaway"`
	Token   string `yaml:"token"`
}

type machine struct {
	Path string `yaml:"path"`
}

func New() *Config {
	workspace := "/opt/armcnc"
	runtime := "/opt/armcnc/runtime"

	envValue := os.Getenv("ARMCNC_WORKSPACE_PATH")
	if envValue == "" {
		workspace = envValue
		runtime = filepath.Join(envValue, "/runtime")
	}

	return &Config{
		Workspace: workspace,
		Runtime:   runtime,
	}
}

func (c *Config) Init() *Config {
	file, err := os.ReadFile(filepath.Join(c.Workspace, "/backend/release/config.sample.yaml"))
	if err != nil {
		return c
	}

	err = yaml.Unmarshal(file, c)
	if err != nil {
		return c
	}

	return c
}

func (c *Config) Update() *Config {
	data, err := yaml.Marshal(c)
	if err != nil {
		return c
	}

	configDir := filepath.Join(c.Workspace, "/backend/release")
	if err := os.MkdirAll(configDir, 0755); err != nil {
		return c
	}

	configPath := filepath.Join(configDir, "config.sample.yaml")
	if err := os.WriteFile(configPath, data, 0644); err != nil {
		return c
	}

	return c
}
