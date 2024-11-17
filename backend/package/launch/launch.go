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

package launch

import (
	"os"
	"os/exec"
	"path/filepath"
	"time"

	"github.com/armcnc/armcnc/backend/utils"
	fileUtils "github.com/armcnc/armcnc/backend/utils/file"
)

var Get = &Launch{}

type Launch struct{}

func New() *Launch {
	return &Launch{}
}

func (launch *Launch) Start() {
	exist, _ := fileUtils.PathExist("/tmp/linuxcnc.lock")
	if !exist {
		launch.Remove()
		cmd := exec.Command("systemctl", "start", "armcnc_linuxcnc.service")
		_, err := cmd.Output()
		if err != nil {
			return
		}
		time.Sleep(2 * time.Second)
		cmd = exec.Command("systemctl", "start", "armcnc_launch.service")
		_, err = cmd.Output()
		if err != nil {
			return
		}
		go func() {
			for !utils.IsGraphicalTargetActive() {
				time.Sleep(1 * time.Second)
			}
			cmd = exec.Command("systemctl", "start", "armcnc_chromium.service")
			_, err := cmd.Output()
			if err != nil {
				return
			}
		}()
	} else {
		launch.Restart()
	}
}

func (launch *Launch) Restart() {
	launch.Remove()
	cmd := exec.Command("systemctl", "restart", "armcnc_linuxcnc.service")
	_, err := cmd.Output()
	if err != nil {
		return
	}
	time.Sleep(2 * time.Second)
	cmd = exec.Command("systemctl", "restart", "armcnc_launch.service")
	_, err = cmd.Output()
	if err != nil {
		return
	}
}

func (launch *Launch) Remove() {
	files, err := filepath.Glob(filepath.Join("/tmp/", "linuxcnc.*"))
	if err == nil {
		for _, file := range files {
			err := os.Remove(file)
			if err != nil {
				return
			}
		}
	}
}

func (launch *Launch) Stop() {
	cmd := exec.Command("systemctl", "stop", "armcnc_launch.service")
	_, err := cmd.Output()
	if err != nil {
		return
	}
	cmd = exec.Command("systemctl", "stop", "armcnc_linuxcnc.service")
	_, err = cmd.Output()
	if err != nil {
		return
	}
	cmd = exec.Command("systemctl", "stop", "armcnc_chromium.service")
	_, err = cmd.Output()
	if err != nil {
		return
	}
}
