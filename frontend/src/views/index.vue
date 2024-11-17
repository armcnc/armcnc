<template>
    <div class="fixed w-[1280px] h-[800px] mx-auto top-[calc(50%-400px)] left-0 right-0 border border-zinc-900 z-[1000]">
        <div class="grid gap-0 grid-cols-1 absolute top-0 left-0 right-0 bottom-0 w-full h-full bg-zinc-950 z-[2000]" v-if="!props.data.backend.status">
            <div class="w-5 h-5 m-auto items-center">
                <svg class="animate-spin h-5 w-5" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24">
                    <circle class="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" stroke-width="4"></circle>
                    <path class="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4zm2 5.291A7.962 7.962 0 014 12H0c0 3.042 1.135 5.824 3 7.938l3-2.647z"></path>
                </svg>
            </div>
        </div>
        <div class="relative flex w-full h-full flex-col items-center" v-else>
            <div class="w-full h-[65px] border-b border-zinc-900">
                <CommonHeader ref="commonHeader" :data="props.data" />
            </div>
            <div class="w-full h-[calc(100%-100px)]">
                <div class="w-full h-full flex flex-row">
                    <div class="w-[65px] border-r border-zinc-900">
                        <div class="w-full h-full space-y-5 py-2">
                            <div class="w-full">
                                <div class="w-12 h-12 bg-muted/0 rounded-md py-[12px] mx-auto text-muted-foreground" :class="props.data.current === 'dashboard' ? 'bg-muted/50 text-white' : ''" @click="onCurrent('dashboard')">
                                    <GaugeIcon class="w-6 h-6 mx-auto" />
                                </div>
                            </div>
                            <div class="w-full">
                                <div class="w-12 h-12 bg-muted/0 rounded-md py-[12px] mx-auto text-muted-foreground" :class="props.data.current === 'machine' ? 'bg-muted/50 text-white' : ''" @click="onCurrent('machine')">
                                    <FileCogIcon class="w-6 h-6 mx-auto" />
                                </div>
                            </div>
                            <div class="w-full">
                                <div class="w-12 h-12 bg-muted/0 rounded-md py-[12px] mx-auto text-muted-foreground" :class="props.data.current === 'program' ? 'bg-muted/50 text-white' : ''" @click="onCurrent('program')">
                                    <FileCode2Icon class="w-6 h-6 mx-auto" />
                                </div>
                            </div>
                            <div class="w-full">
                                <div class="w-12 h-12 bg-muted/0 rounded-md py-[12px] mx-auto text-muted-foreground" :class="props.data.current === 'plugin' ? 'bg-muted/50 text-white' : ''" @click="onCurrent('plugin')">
                                    <LayoutGridIcon class="w-6 h-6 mx-auto" />
                                </div>
                            </div>
                            <div class="w-full">
                                <div class="w-12 h-12 bg-muted/0 rounded-md py-[12px] mx-auto text-muted-foreground" :class="props.data.current === 'store' ? 'bg-muted/50 text-white' : ''" @click="onCurrent('store')">
                                    <ShoppingBagIcon class="w-6 h-6 mx-auto" />
                                </div>
                            </div>
                        </div>
                    </div>
                    <div class="w-[calc(100%-65px)]">
                        <MiddleDashboard ref="middleDashboard" :data="props.data" v-if="props.data.current === 'dashboard'" />
                    </div>
                </div>
            </div>
            <div class="w-full h-[35px] border-t border-zinc-900">
                <CommonFooter ref="commonFooter" :data="props.data" />
            </div>
        </div>
    </div>
</template>

<script setup lang="ts">
import {nextTick, onBeforeMount, onBeforeUnmount, onMounted, onUnmounted} from "vue";
import CommonHeader from "./common/header.vue";
import CommonFooter from "./common/footer.vue";
import MiddleDashboard from "./middle/dashboard.vue";
import {GaugeIcon, FileCogIcon, FileCode2Icon, LayoutGridIcon, ShoppingBagIcon} from "lucide-vue-next";

const props: any = defineProps<{
    data: any
}>();

function onCurrent(value: string){
    if(props.data.current !== value){
        props.data.current = value;
    }
}

function onDisconnectDevice(){
    if(props.data.backend.socket.status){
        props.data.backend.socket.connect.close();
        props.data.backend.socket.connect = false;
        props.data.backend.socket.status = false;
    }
    props.data.current = "dashboard";
}

function onProgram(file: string){
    props.data.tools.request("/backend/program/read", "GET", {file: file}, {}).then((request: any) => {
        if(request.status === 200){
            if (request.data.code === 0) {
                const data = request.data.data;
                props.data.program.lines = data.lines;
                if((window as any).simulation){
                    (window as any).simulation.clearToolLine();
                    (window as any).simulation.onLoadCode(props.data.program.lines);
                }
            }else{
                props.data.tools.toast({
                    title: props.data.tools.language.t("status.toast.title"),
                    description: props.data.tools.language.t("status." + props.data.tools.language.f(request.data.message)),
                    variant: "warning",
                });
            }
        }else{
            props.data.tools.toast({
                title: props.data.tools.language.t("status.toast.title"),
                description: props.data.tools.language.t("status.service_exception"),
                variant: "error",
            });
        }
    });
}

function onSocket(){
    const host = window.location.hostname;
    if(!props.data.backend.socket.status){
        props.data.backend.socket.connect = new WebSocket(props.data.mode ? `ws://${__VITE_DEV_HOST__}:1081/message/index` : `ws://${host}:1081/message/index`, undefined);
        props.data.backend.socket.connect.onopen = function () {
            props.data.backend.socket.status = true;
            props.data.current = "dashboard";
        }
        props.data.backend.socket.connect.onmessage = function (message: any) {
            let message_json: {command: string, data: any, message: any} = JSON.parse(message.data);
            if(message_json.command){
                if(message_json.command === "machine:restart"){
                    if(message_json.data){
                        props.data.machine.is_first = true;
                    }
                    if(!message_json.data){
                        props.data.machine.is_first = true;
                    }
                }
                if(message_json.command === "machine:program:open"){
                    if(message_json.data.status){
                        onProgram(message_json.data.file);
                    }else{
                        props.data.tools.toast({
                            title: props.data.tools.language.t("status.toast.title"),
                            description: "程序加载失败",
                            variant: "warning",
                        });
                    }
                }
                if(message_json.command === "machine:data"){
                    props.data.backend.status = true;
                    props.data.machine.data = message_json.data;
                    props.data.machine.path = props.data.machine.data.format_data.machine_path;
                    props.data.machine.current_velocity = parseFloat(props.data.machine.data.format_data.current_velocity) * 60;
                    props.data.machine.velocity = props.data.machine.data.format_data.data.velocity;
                    props.data.machine.g92_offset = props.data.machine.data.format_data.data.g92_offset;
                    props.data.machine.g5x_offset = props.data.machine.data.format_data.data.g5x_offset;
                    props.data.machine.g_offset = props.data.machine.data.format_data.data.g_offset;
                    props.data.machine.dtg_offset = props.data.machine.data.format_data.data.dtg_offset;
                    props.data.machine.tool = props.data.machine.data.format_data.data.tool;
                    props.data.machine.time = props.data.machine.data.format_data.time;
                    props.data.machine.axes = [];
                    props.data.machine.data.format_data.axes.forEach((item: any, index: any, array: any)=>{
                        props.data.machine.axes.push({
                            name: item, 
                            index: index, 
                            position: parseFloat(props.data.machine.data.format_data.data.position[index]), 
                            home: props.data.machine.data.format_data.homed[index]
                        });
                    });
                    props.data.machine.offset.index = props.data.machine.data.format_data.data.index;
                    props.data.machine.offset.options = props.data.machine.data.format_data.data.options;
                    props.data.machine.spindle.enabled = props.data.machine.data.format_data.spindle.enabled;
                    props.data.machine.spindle.direction = props.data.machine.data.format_data.spindle.direction;
                    if(props.data.machine.data.state === 2){
                        props.data.machine.spindle.speed = props.data.machine.data.format_data.spindle.speed;
                    }
                    if(props.data.machine.is_first){
                        props.data.machine.linear_units = props.data.machine.data.format_data.linear_units;
                        props.data.machine.angular_units = props.data.machine.data.format_data.angular_units;
                        props.data.machine.rocker.items.forEach((item: any, index: any, array: any)=>{
                            if(index == 1){
                                if(props.data.machine.data.format_data.axes.includes("Y")){
                                    props.data.machine.rocker.items[index].name = "Y";
                                    props.data.machine.rocker.items[index].value = "y+";
                                    props.data.machine.rocker.items[index].hide = false
                                }
                            }
                            if(index == 3){
                                if(props.data.machine.data.format_data.axes.includes("Z")){
                                    props.data.machine.rocker.items[index].name = "Z";
                                    props.data.machine.rocker.items[index].value = "z+";
                                    props.data.machine.rocker.items[index].hide = false
                                }
                            }
                            if(index == 4){
                                if(props.data.machine.data.format_data.axes.includes("X")){
                                    props.data.machine.rocker.items[index].name = "X";
                                    props.data.machine.rocker.items[index].value = "x-";
                                    props.data.machine.rocker.items[index].hide = false
                                }
                            }
                            if(index == 5){
                                if(props.data.machine.data.format_data.axes.includes("Y")){
                                    props.data.machine.rocker.items[index].name = "Y";
                                    props.data.machine.rocker.items[index].value = "y-";
                                    props.data.machine.rocker.items[index].hide = false
                                }
                            }
                            if(index == 6){
                                if(props.data.machine.data.format_data.axes.includes("X")){
                                    props.data.machine.rocker.items[index].name = "X";
                                    props.data.machine.rocker.items[index].value = "x+";
                                    props.data.machine.rocker.items[index].hide = false
                                }
                            }
                            if(index == 7){
                                if(props.data.machine.data.format_data.axes.includes("X")){
                                    props.data.machine.rocker.items[index].name = "Z";
                                    props.data.machine.rocker.items[index].value = "z-";
                                    props.data.machine.rocker.items[index].hide = false
                                }
                            }
                            if(index == 8){
                                if(props.data.machine.data.format_data.axes.length > 3){
                                    props.data.machine.rocker.items[index].name = props.data.machine.data.format_data.axes[3];
                                    props.data.machine.rocker.items[index].value = props.data.machine.data.format_data.axes[3].toLowerCase() + "-";
                                    props.data.machine.rocker.items[index].hide = false
                                }
                            }
                            if(index == 9){
                                if(props.data.machine.data.format_data.axes.length > 3){
                                    props.data.machine.rocker.items[index].name = props.data.machine.data.format_data.axes[3];
                                    props.data.machine.rocker.items[index].value = props.data.machine.data.format_data.axes[3].toLowerCase() + "+";
                                    props.data.machine.rocker.items[index].hide = false
                                }
                            }
                            if(index == 10){
                                if(props.data.machine.data.format_data.axes.length > 4){
                                    props.data.machine.rocker.items[index].name = props.data.machine.data.format_data.axes[4];
                                    props.data.machine.rocker.items[index].value = props.data.machine.data.format_data.axes[4].toLowerCase() + "-";
                                    props.data.machine.rocker.items[index].hide = false
                                }
                            }
                            if(index == 11){
                                if(props.data.machine.data.format_data.axes.length > 4){
                                    props.data.machine.rocker.items[index].name = props.data.machine.data.format_data.axes[4];
                                    props.data.machine.rocker.items[index].value = props.data.machine.data.format_data.axes[4].toLowerCase() + "+";
                                    props.data.machine.rocker.items[index].hide = false
                                }
                            }
                        });
                        props.data.machine.spindle.min_velocity = props.data.machine.data.format_data.spindle.min_velocity;
                        props.data.machine.spindle.max_velocity = props.data.machine.data.format_data.spindle.max_velocity;
                        props.data.machine.spindle.min_override = props.data.machine.data.format_data.spindle.min_override * 100;
                        props.data.machine.spindle.max_override = props.data.machine.data.format_data.spindle.max_override * 100;
                        props.data.machine.spindle.override = props.data.machine.data.format_data.spindle.override * 100;
                        props.data.machine.spindle.default_speed = props.data.machine.data.format_data.spindle.default_speed;
                        props.data.machine.spindle.speed = props.data.machine.data.format_data.spindle.default_speed;
                        props.data.machine.feed.max_override = props.data.machine.data.format_data.feed.max_override * 100;
                        props.data.machine.feed.override = props.data.machine.data.format_data.feed.override * 100;
                        props.data.machine.max_velocity = props.data.machine.data.format_data.max_velocity * 60;
                        props.data.machine.max_linear_velocity = Math.round(props.data.machine.data.format_data.max_linear_velocity * 60);
                        props.data.machine.default_linear_velocity = Math.round(props.data.machine.data.format_data.default_linear_velocity * 60);
                        props.data.machine.max_angular_velocity = Math.round(props.data.machine.data.format_data.max_angular_velocity * 60);
                        props.data.machine.default_angular_velocity = Math.round(props.data.machine.data.format_data.default_angular_velocity * 60);
                        if(props.data.machine.data.file === ""){
                            props.data.machine.data.file = "armcnc.ngc";
                            let file_part = props.data.machine.data.file.split("/");
                            props.data.machine.file = file_part.pop();
                            let message = {command: "client:machine:program:open", data: props.data.machine.data.file};
                            props.data.backend.socket.connect.send(JSON.stringify(message));
                        }else{
                            let file_part = props.data.machine.data.file.split("/");
                            props.data.machine.file = file_part.pop();
                            onProgram(props.data.machine.file);
                        }
                        props.data.machine.is_first = false;
                    }
                    if(props.data.machine.data.format_data.increments){
                        props.data.machine.step.items = [];
                        props.data.machine.data.format_data.increments.forEach((item: any) => {
                            props.data.machine.step.items.push({label: item.replace("mm", ""), value: parseFloat(item.replace("mm", ""))});
                        });
                        props.data.machine.step.items.push({label: "连续", value: -1});
                    }
                    props.data.machine.state.limit = "allow";
                    props.data.machine.homed = props.data.machine.data.format_data.is_homed;
                    if(props.data.machine.data.format_data.task_state == 1){
                        props.data.machine.state.estop = "disabled";
                        props.data.machine.state.enabled = "disabled";
                        props.data.machine.state.start = "disabled";
                        props.data.machine.state.start_disabled = false;
                        props.data.machine.state.pause = "disabled";
                        props.data.machine.state.pause_disabled = true;
                        props.data.machine.state.stop = "disabled";
                        props.data.machine.state.stop_disabled = true;
                        props.data.machine.home = "disabled";
                        props.data.machine.relative_offset = "disabled";
                    }
                    if(props.data.machine.data.format_data.task_state == 2){
                        props.data.machine.state.estop = "active";
                        props.data.machine.state.enabled = "allow";
                        props.data.machine.state.start = "allow";
                        props.data.machine.state.start_disabled = true;
                        props.data.machine.state.pause = "disabled";
                        props.data.machine.state.pause_disabled = true;
                        props.data.machine.state.stop = "disabled";
                        props.data.machine.state.stop_disabled = true;
                        props.data.machine.home = "disabled";
                        props.data.machine.relative_offset = "disabled";
                    }
                    if(props.data.machine.data.format_data.task_state == 4){
                        props.data.machine.state.estop = "active";
                        props.data.machine.state.enabled = "active";
                        if(props.data.machine.data.paused){
                            props.data.machine.state.pause = "active";
                            props.data.machine.state.pause_disabled = false;
                            props.data.machine.state.start = "active";
                            props.data.machine.state.start_disabled = true;
                            props.data.machine.state.stop = "allow";
                            props.data.machine.state.stop_disabled = false;
                        }else{
                            if(props.data.machine.data.state === 2){
                                props.data.machine.state.start = "active";
                                props.data.machine.state.start_disabled = true;
                                props.data.machine.state.pause = "allow";
                                props.data.machine.state.pause_disabled = false;
                                props.data.machine.state.stop = "allow";
                                props.data.machine.state.stop_disabled = false;
                            }
                            if(props.data.machine.data.state === 1 || props.data.machine.data.state === 3){
                                props.data.machine.state.stop = "active";
                                props.data.machine.state.stop_disabled = true;
                                props.data.machine.state.start = "allow";
                                props.data.machine.state.start_disabled = false;
                                props.data.machine.state.pause = "allow";
                                props.data.machine.state.pause_disabled = true;
                            }
                        }
                        props.data.machine.home = "allow";
                        props.data.machine.relative_offset = "allow";
                    }
                    // TODO 刀路实时渲染
                    props.data.machine.program.line = props.data.machine.data.motion_line;
                }
                if(message_json.command === "machine:error"){
                    let kind = [11];
                    if(kind.includes(message_json.data)){
                        let new_message = message_json.message.split(":");
                        props.data.tools.toast({
                            title: props.data.tools.language.t("status.toast.title"),
                            description: (new_message.length > 1 ? new_message[1] : message_json.message),
                            variant: "warning",
                        });
                    }
                }
            }
        };
        props.data.backend.socket.connect.onerror = function () {
            if(props.data.backend.socket.status){
                onDisconnectDevice();
            }
        }
        props.data.backend.socket.connect.onclose = function () {
            if(props.data.backend.socket.status){
                onDisconnectDevice();
            }
        }
    }
}

function onService(){
    props.data.tools.request("/backend", "GET", {}, {}).then((request: any) => {
        if(request.status === 200){
            if (request.data.code === 0) {
                const data = request.data.data;
                onSocket();
            }else{
                props.data.tools.toast({
                    title: props.data.tools.language.t("status.toast.title"),
                    description: props.data.tools.language.t("status." + props.data.tools.language.f(request.data.message)),
                    variant: "warning",
                });
            }
        }else{
            props.data.tools.toast({
                title: props.data.tools.language.t("status.toast.title"),
                description: props.data.tools.language.t("status.service_exception"),
                variant: "error",
            });
        }
    });
}

onBeforeMount(() => {});

onMounted(() => {
    nextTick(() => {
        console.log("[pages:index]", props);
        props.data.version = __APP_NAME__ + " " + __APP_VERSION__;
        document.onkeydown = function(event: any) {
            if(event.key === "Tab"){
                event.preventDefault();
            }
        }
        onService();
    });
});

onBeforeUnmount(() => {
    onDisconnectDevice();
});

onUnmounted(() => {});
</script>
