<template>
    <div class="fixed w-[1280px] h-[800px] mx-auto top-[calc(50%-400px)] left-0 right-0 border border-zinc-900 z-[10000]">
        <div class="grid gap-0 grid-cols-1 absolute top-0 left-0 right-0 bottom-0 w-full h-full bg-zinc-950 z-[50000]" v-if="!props.data.backend.status">
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
                                <div class="w-12 h-12 bg-muted/0 rounded-md py-[12px] mx-auto text-muted-foreground" :class="props.data.middle.current === 'dashboard' ? 'bg-muted/50 text-white' : ''" @click="onMiddleCurrent('dashboard')">
                                    <GaugeIcon class="w-6 h-6 mx-auto" />
                                </div>
                            </div>
                            <div class="w-full">
                                <div class="w-12 h-12 bg-muted/0 rounded-md py-[12px] mx-auto text-muted-foreground" :class="props.data.middle.current === 'machine' ? 'bg-muted/50 text-white' : ''" @click="onMiddleCurrent('machine')">
                                    <FileCogIcon class="w-6 h-6 mx-auto" />
                                </div>
                            </div>
                            <div class="w-full">
                                <div class="w-12 h-12 bg-muted/0 rounded-md py-[12px] mx-auto text-muted-foreground" :class="props.data.middle.current === 'program' ? 'bg-muted/50 text-white' : ''" @click="onMiddleCurrent('program')">
                                    <FileCode2Icon class="w-6 h-6 mx-auto" />
                                </div>
                            </div>
                            <div class="w-full">
                                <div class="w-12 h-12 bg-muted/0 rounded-md py-[12px] mx-auto text-muted-foreground" :class="props.data.middle.current === 'plugin' ? 'bg-muted/50 text-white' : ''" @click="onMiddleCurrent('plugin')">
                                    <LayoutGridIcon class="w-6 h-6 mx-auto" />
                                </div>
                            </div>
                            <div class="w-full">
                                <div class="w-12 h-12 bg-muted/0 rounded-md py-[12px] mx-auto text-muted-foreground" :class="props.data.middle.current === 'store' ? 'bg-muted/50 text-white' : ''" @click="onMiddleCurrent('store')">
                                    <ShoppingBagIcon class="w-6 h-6 mx-auto" />
                                </div>
                            </div>
                        </div>
                    </div>
                    <div class="w-[calc(100%-65px)]">
                        <MiddleDashboard ref="middleIndex" :data="props.data" v-if="props.data.middle.current === 'dashboard'" />
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

function onMiddleCurrent(value: string){
    if(props.data.middle.current !== value){
        props.data.middle.current = value;
    }
}

function onDisconnectDevice(){
    if(props.data.backend.socket.status){
        props.data.backend.socket.connect.close();
        props.data.backend.socket.connect = false;
        props.data.backend.socket.status = false;
    }
    props.data.middle.current = "console";
}

function onSocket(){
    const protocol = window.location.protocol === "https:" ? "wss" : "ws";
    const host = window.location.hostname;
    const port = window.location.port;
    if(!props.data.backend.socket.status){
        props.data.backend.socket.connect = new WebSocket(props.data.mode ? "ws://192.168.31.58:1081/message/index" : `${protocol}://${host}${port ? ':' + port : ''}/message/index`, undefined);
        props.data.backend.socket.connect.onopen = function () {
            props.data.backend.socket.status = true;
        }
        props.data.backend.socket.connect.onmessage = function (message: any) {
            let message_json = JSON.parse(message.data);
            console.log(message_json);
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
                props.data.backend.status = true;
                setTimeout(()=>{
                    props.data.screen.width = window.innerWidth;
                    props.data.screen.height = window.innerHeight;
                }, 5000);
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
