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

import Axios from "axios";
import FingerprintJS from "@fingerprintjs/fingerprintjs";

const request = Axios.create({
    baseURL: "",
    timeout: 5000,
});

request.interceptors.response.use(
    response => {
        if (response.status === 200) {
            return Promise.resolve(response);
        } else {
            return Promise.reject(response);
        }
    },
    error => {
        if(error.response){
            if (error.response.status) {
                return false;
            }
        }
        return false;
    }
);


export async function Request(path: string, method: string, params: object, data: object) {
    return FingerprintJS.load().then((fp: any) => {
        return fp.get().then((result: any) => {
            return request({
                baseURL: "",
                headers: {
                    "Content-Type": "application/json",
                    "Content-X-Time": Date.now().toString(),
                    "Content-X-Device": result.visitorId,
                    "Content-X-Referer":"armcnc.net",
                    "Content-X-Source": "browser",
                    "Content-X-IP": "0.0.0.0",
                    "Content-X-Sign": localStorage.getItem("armcnc:login:token") ? localStorage.getItem("armcnc:login:token") : ""
                },
                url: path,
                method: method,
                params: params ? params : {},
                data: data ? data : {}
            });
        });
    }).catch(()=>{
        return request({
            baseURL: "",
            headers: {
                "Content-Type": "application/json",
                "Content-X-Time": Date.now().toString(),
                "Content-X-Device": "",
                "Content-X-Referer":"armcnc.net",
                "Content-X-Source": "browser",
                "Content-X-IP": "0.0.0.0",
                "Content-X-Sign": ""
            },
            url: path,
            method: method,
            params: params ? params : {},
            data: data ? data : {}
        });
    });
}