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
                <Button class="text-muted-foreground" variant="outline" size="lg">
                    <PlayIcon class="w-4 h-4" />
                </Button>
            </div>
            <div class="w-auto">
                <Button class="text-muted-foreground" variant="outline" size="lg">
                    <PauseIcon class="w-4 h-4" />
                </Button>
            </div>
            <div class="w-auto">
                <Button class="text-muted-foreground" variant="outline" size="lg">
                    <OctagonMinusIcon class="w-4 h-4" />
                </Button>
            </div>
        </div>
        <div class="w-full flex flex-row items-center justify-end space-x-2 pr-[14px]">
            <div class="w-auto">
                <Button class="px-7 text-muted-foreground" variant="outline" size="lg">
                    <PowerOffIcon class="w-4 h-4 mr-2 text-red-500" />
                    <span>急停</span>
                </Button>
            </div>
            <div class="w-auto">
                <Button class="px-7 text-muted-foreground" variant="outline" size="lg">
                    <PowerIcon class="w-4 h-4 mr-2 text-primary" />
                    <span>启动</span>
                </Button>
            </div>
            <div class="w-auto">
                <Button class="px-7 text-muted-foreground" variant="outline" size="lg">
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

onBeforeMount(() => {});

onMounted(() => {
    nextTick(() => {
        console.log("[pages:common:header]", props);
    });
});

onBeforeUnmount(() => {});

onUnmounted(() => {});
</script>
