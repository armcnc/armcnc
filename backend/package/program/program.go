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

package program

import (
	"bufio"
	"encoding/json"
	"github.com/armcnc/armcnc/backend/utils/file"
	"github.com/djherbis/times"
	"os"
	"path/filepath"
	"sort"
	"strings"
	"time"
)

var Get = &Program{}

type Program struct {
	Path string `json:"path"`
}

type Item struct {
	Name     string    `json:"name"`
	Path     string    `json:"path"`
	Describe string    `json:"describe"`
	Version  string    `json:"version"`
	Line     []string  `json:"line"`
	Content  string    `json:"content"`
	Time     time.Time `json:"-"`
}

func New(workspace string) *Program {
	return &Program{
		Path: filepath.Join(workspace, "/programs"),
	}
}

func (program *Program) Select() []Item {
	items := make([]Item, 0)

	files, err := os.ReadDir(program.Path)
	if err != nil {
		return items
	}

	for _, file := range files {
		item := Item{}
		if !file.IsDir() {
			item.Path = file.Name()
			timeData, _ := times.Stat(program.Path + file.Name())
			item.Time = timeData.BirthTime()
			if file.Name() == "armcnc.ngc" || file.Name() == "demo.ngc" || file.Name() == "linuxcnc.ngc" {
				item.Time = item.Time.Add(-525600 * time.Minute)
			}
			firstLine := program.ReadFirstLine(file.Name())
			if firstLine.Version != "" {
				item.Name = firstLine.Name
				item.Describe = firstLine.Describe
				item.Version = firstLine.Version
				item.Line = make([]string, 0)
				item.Content = ""
				items = append(items, item)
			}
		}
	}
	sort.Slice(items, func(i, j int) bool {
		return items[i].Time.After(items[j].Time)
	})
	return items
}

func (program *Program) ReadContent(fileName string) string {
	content := ""
	exists, _ := fileUtils.PathExist(program.Path + fileName)
	if exists {
		contentByte, err := fileUtils.ReadFile(program.Path + fileName)
		if err == nil {
			content = string(contentByte)
		}
	}
	return content
}

func (program *Program) ReadLine(fileName string) Item {
	item := Item{}
	item.Line = make([]string, 0)

	file, err := os.Open(program.Path + fileName)
	if err != nil {
		return item
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := scanner.Text()
		item.Line = append(item.Line, line)
	}
	return item
}

func (program *Program) ReadFirstLine(fileName string) Item {
	item := Item{}

	file, err := os.Open(program.Path + fileName)
	if err != nil {
		return item
	}
	defer file.Close()

	reader := bufio.NewReader(file)
	line, err := reader.ReadString('\n')
	if err != nil {
		return item
	}

	line = strings.TrimSpace(line)
	if len(line) > 0 && line[0] == '(' && line[len(line)-1] == ')' {
		jsonStr := line[1 : len(line)-1]
		err := json.Unmarshal([]byte(jsonStr), &item)
		if err != nil {
			return item
		}
	}
	return item
}

func (program *Program) UpdateContent(fileName string, content string) bool {
	status := false
	exists, _ := fileUtils.PathExist(program.Path + fileName)
	if exists {
		write := fileUtils.WriteFile(content, program.Path+fileName)
		if write == nil {
			status = true
		}
	}
	return status
}

func (program *Program) Delete(fileName string) bool {
	status := false
	exists, _ := fileUtils.PathExist(program.Path + fileName)
	if exists {
		err := os.RemoveAll(program.Path + fileName)
		if err == nil {
			status = true
		}
	}
	return status
}
