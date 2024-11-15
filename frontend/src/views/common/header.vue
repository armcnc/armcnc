<template>
    <div class="w-full h-full flex flex-row items-center">
        <div class="w-full flex flex-row items-center space-x-2">
            <div class="w-[65px]">
                <img src="/images/logo.png" class="w-10 mx-auto" alt="" @click="onRefresh">
            </div>
            <Button class="text-muted-foreground" variant="outline" size="lg" disabled>
                <Link2Icon class="w-4 h-4 mr-2" />
                <span>本机设备</span>
            </Button>
        </div>
        <div class="w-full flex flex-row items-center justify-center space-x-3">
            <div class="w-auto">
                <Button class="text-muted-foreground hover:bg-transparent active:bg-primary" variant="outline" size="lg" :disabled="props.data.machine.state.start === 'disabled' || props.data.machine.state.start_disabled" @click="onControlStart(0)">
                    <PlayIcon class="w-4 h-4" />
                </Button>
            </div>
            <div class="w-auto">
                <Button class="text-muted-foreground hover:bg-transparent active:bg-primary" variant="outline" size="lg" :disabled="props.data.machine.state.pause === 'disabled' || props.data.machine.state.pause_disabled" @click="onControlPause">
                    <PauseIcon class="w-4 h-4" />
                </Button>
            </div>
            <div class="w-auto">
                <Button class="text-muted-foreground hover:bg-transparent active:bg-primary" variant="outline" size="lg" :disabled="props.data.machine.state.stop === 'disabled' || props.data.machine.state.stop_disabled" @click="onControlStop">
                    <OctagonMinusIcon class="w-4 h-4" />
                </Button>
            </div>
        </div>
        <div class="w-full flex flex-row items-center justify-end space-x-2 pr-[14px]">
            <div class="w-auto">
                <Button class="px-7 text-muted-foreground" variant="outline" size="lg" :class="props.data.machine.state.estop" :disabled="props.data.machine.state.estop === 'disabled'" @click="onEmergencyStop">
                    <PowerOffIcon class="w-4 h-4 mr-2 text-red-500" />
                    <span>急停</span>
                </Button>
            </div>
            <div class="w-auto">
                <Button class="px-7" variant="outline" size="lg" :class="props.data.machine.state.enabled === 'active' ? 'bg-primary hover:bg-primary' : 'text-muted-foreground hover:bg-transparent'" :disabled="props.data.machine.state.enabled === 'disabled'" @click="onDeviceStart">
                    <PowerIcon class="w-4 h-4 mr-2" />
                    <span>启动</span>
                </Button>
            </div>
            <div class="w-auto">
                <Button class="px-7 text-muted-foreground hover:bg-transparent active:bg-primary" variant="outline" size="lg" :class="props.data.machine.state.limit" :disabled="props.data.machine.state.limit === 'disabled' || props.data.machine.data.state === 2" @click="onDeviceOverrideLimits">
                    <MapPinOffIcon class="w-4 h-4 mr-2 text-amber-500" />
                    <span>忽略限位</span>
                </Button>
            </div>
        </div>
    </div>
</template>

<script setup lang="ts">
import {nextTick, onBeforeMount, onBeforeUnmount, onMounted, onUnmounted} from "vue";
import {Button} from "../../packages/york";
import {Link2Icon, MapPinOffIcon, PlayIcon, PowerIcon, PowerOffIcon, PauseIcon, OctagonMinusIcon} from "lucide-vue-next";

const props: any = defineProps<{
    data: any
}>();

function onRefresh(){
    location.reload();
}

function onControlStart(line: any){
    if(props.data.machine.data){
        if ((props.data.machine.data.state == 2 && props.data.machine.data.paused != true) || props.data.machine.data.enabled != true) {
            return;
        }
        if(!props.data.machine.data.format_data.is_homed){
            return;
        }
        // if(props.cnc.console.bottom.line.motion_line > 0 && props.cnc.header.centre.pause !== "active"){
        //     if((window as any).simulation){
        //         (window as any).simulation.clearToolLine();
        //     }
        // }
        let message = {command: "client:machine:start", data: {line: line}}
        props.data.backend.socket.connect.send(JSON.stringify(message));
        // (window as any).runtime.EventsEmit("set_start_line", {});
    }
}

function onControlPause(){
    if(props.data.machine.data){
        if (props.data.machine.pause === "active"){
            onControlStart(0);
        }
        if ((props.data.machine.data.state == 2 && props.data.machine.data.paused == true) || props.data.machine.data.state != 2) {
            return;
        }
        let message = {command: "client:machine:pause", data: {}}
        props.data.backend.socket.connect.send(JSON.stringify(message));
    }
}

function onControlStop(){
    if(props.data.machine.data){
        if (props.data.machine.data.state == 2) {
            let message = {command: "client:machine:stop", data: {}}
            props.data.backend.socket.connect.send(JSON.stringify(message));
        }
    }
}

function onEmergencyStop(){
    if(props.data.machine.data){
        let message = {command: "client:machine:estop"}
        props.data.backend.socket.connect.send(JSON.stringify(message));
    }else{
        props.data.tools.toast({
            title: props.data.tools.language.t("status.toast.title"),
            description: "设备未就绪",
            variant: "warning",
        });
    }
}

function onDeviceStart(){
    if(props.data.machine.data){
        let message = {command: "client:machine:device:start"}
        props.data.backend.socket.connect.send(JSON.stringify(message));
    }else{
        props.data.tools.toast({
            title: props.data.tools.language.t("status.toast.title"),
            description: "设备未就绪",
            variant: "warning",
        });
    }
}

function onDeviceOverrideLimits(){
    if(props.data.machine.data && props.data.machine.data.state !== 2){
        let message = {command: "client:machine:device:override_limits"}
        props.data.backend.socket.connect.send(JSON.stringify(message));
    }else{
        props.data.tools.toast({
            title: props.data.tools.language.t("status.toast.title"),
            description: "设备未就绪",
            variant: "warning",
        });
    }
}


onBeforeMount(() => {});

onMounted(() => {
    nextTick(() => {
        console.log("[pages:common:header]", props);
    });
});

onBeforeUnmount(() => {});

onUnmounted(() => {});
</script>
