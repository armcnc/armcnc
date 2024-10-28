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

package command

import (
	"github.com/armcnc/armcnc/backend/package/config"
	"github.com/armcnc/armcnc/backend/package/version"
	"github.com/armcnc/armcnc/backend/service"
	"github.com/armcnc/armcnc/backend/service/crontab"
	"github.com/spf13/cobra"
)

func Service() *cobra.Command {
	command := &cobra.Command{
		Use:     "service",
		Short:   "Start service module",
		Long:    "Start service module",
		Example: "armcnc service",
		Run:     serviceRun,
	}
	return command
}

func serviceRun(cmd *cobra.Command, args []string) {
	version.Get = version.New()
	service.Get = service.New()
	config.Get = config.New().LoadConfig()
	crontab.Get = crontab.New().Start()
	service.Get.Start()
}
