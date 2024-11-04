<template>
    <div class="w-full h-full flex flex-row items-center px-2">
        <div class="w-full flex flex-row items-center space-x-2">
            <div class="w-auto">
                <img src="/images/logo.png" class="w-10" alt="">
            </div>
            <Button class="text-muted-foreground" variant="outline" size="default">
                <Link2Icon class="w-4 h-4 mr-2" />
                <span>本机设备</span>
            </Button>
        </div>
        <div class="w-full flex flex-row items-center justify-center space-x-3">
            <div class="w-auto">
                <Button class="text-muted-foreground" variant="outline" size="default">
                    <PlayIcon class="w-4 h-4" />
                </Button>
            </div>
            <div class="w-auto">
                <Button class="text-muted-foreground" variant="outline" size="default">
                    <PauseIcon class="w-4 h-4" />
                </Button>
            </div>
            <div class="w-auto">
                <Button class="text-muted-foreground" variant="outline" size="default">
                    <OctagonMinusIcon class="w-4 h-4" />
                </Button>
            </div>
        </div>
        <div class="w-full flex flex-row items-center justify-end space-x-2">
            <div class="w-auto">
                <Button class="text-white" variant="destructive" size="default">
                    <PowerOffIcon class="w-4 h-4 mr-2" />
                    <span>急停</span>
                </Button>
            </div>
            <div class="w-auto">
                <Button class="text-muted-foreground" variant="outline" size="default">
                    <PowerIcon class="w-4 h-4 mr-2" />
                    <span>启动</span>
                </Button>
            </div>
            <div class="w-auto">
                <Button class="text-muted-foreground" variant="outline" size="default">
                    <MapPinOffIcon class="w-4 h-4 mr-2" />
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

onBeforeMount(() => {});

onMounted(() => {
    nextTick(() => {
        console.log("[pages:common:header]", props);
    });
});

onBeforeUnmount(() => {});

onUnmounted(() => {});
</script>
