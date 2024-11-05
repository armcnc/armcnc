<template>
    <div class="grid gap-0 grid-cols-1 fixed w-full h-full bg-zinc-950 z-[50000]" v-if="!props.data.backend.status">
        <div class="w-5 h-5 m-auto items-center">
            <svg class="animate-spin h-5 w-5" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24">
                <circle class="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" stroke-width="4"></circle>
                <path class="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4zm2 5.291A7.962 7.962 0 014 12H0c0 3.042 1.135 5.824 3 7.938l3-2.647z"></path>
            </svg>
        </div>
    </div>
    <div class="relative flex w-full h-screen flex-col items-center" v-else>
        <div class="w-full h-[65px] border-b border-zinc-900">
            <CommonHeader ref="commonHeader" :data="props.data" />
        </div>
        <div class="w-full h-[calc(100%-100px)]">
            <div class="w-full h-full flex flex-row">
                <div class="w-[55px] border-r border-zinc-900">
                    <div class="w-full h-full space-y-2 py-2">
                        <div class="w-full">
                            <div class="w-10 h-10 bg-muted/50 rounded-md py-3 mx-auto text-muted-foreground">
                                <GaugeIcon class="w-4 h-4 mx-auto" />
                            </div>
                        </div>
                        <div class="w-full">
                            <div class="w-10 h-10 bg-muted/0 rounded-md py-3 mx-auto text-muted-foreground">
                                <FileCogIcon class="w-4 h-4 mx-auto" />
                            </div>
                        </div>
                        <div class="w-full">
                            <div class="w-10 h-10 bg-muted/0 rounded-md py-3 mx-auto text-muted-foreground">
                                <FileCode2Icon class="w-4 h-4 mx-auto" />
                            </div>
                        </div>
                        <div class="w-full">
                            <div class="w-10 h-10 bg-muted/0 rounded-md py-3 mx-auto text-muted-foreground">
                                <LayoutGridIcon class="w-4 h-4 mx-auto" />
                            </div>
                        </div>
                        <div class="w-full">
                            <div class="w-10 h-10 bg-muted/0 rounded-md py-3 mx-auto text-muted-foreground">
                                <ShoppingBagIcon class="w-4 h-4 mx-auto" />
                            </div>
                        </div>
                    </div>
                </div>
                <div class="w-[calc(100%-55px)]">
                    <MiddleIndex ref="middleIndex" :data="props.data" />
                </div>
            </div>
        </div>
        <div class="w-full h-[35px] border-t border-zinc-900">
            <CommonFooter ref="commonFooter" :data="props.data" />
        </div>
    </div>
</template>

<script setup lang="ts">
import {nextTick, onBeforeMount, onBeforeUnmount, onMounted, onUnmounted} from "vue";
import CommonHeader from "./common/header.vue";
import CommonFooter from "./common/footer.vue";
import MiddleIndex from "./middle/index.vue";
import {GaugeIcon, FileCogIcon, FileCode2Icon, LayoutGridIcon, ShoppingBagIcon} from "lucide-vue-next";

const props: any = defineProps<{
    data: any
}>();

function onService(){
    props.data.tools.request("/backend", "GET", {}, {}).then((request: any) => {
        if(request.status === 200){
            if (request.data.code === 0) {
                const data = request.data.data;
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
        onService();
    });
});

onBeforeUnmount(() => {});

onUnmounted(() => {});
</script>
