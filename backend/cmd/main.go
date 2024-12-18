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

package main

import (
	"fmt"
	"os"

	"github.com/armcnc/armcnc/backend/cmd/command"
	"github.com/armcnc/armcnc/backend/package/config"
	"github.com/armcnc/armcnc/backend/package/version"
	"github.com/spf13/cobra"
)

func main() {

	version.Get = version.New()

	config.Get = config.New().Init()

	cmd := &cobra.Command{
		Use:   version.Get.Name,
		Short: version.Get.Describe,
		Long:  fmt.Sprintf("%s - %s %s (%s)", version.Get.Name, version.Get.Describe, version.Get.Version, version.Get.Site),
	}

	cmd.CompletionOptions.HiddenDefaultCmd = true

	cmd.AddCommand(command.Service())

	cmd.AddCommand(command.Version())

	if err := cmd.Execute(); err != nil {
		os.Exit(1)
	}
}
