<template>
    <div class="w-full h-[65px] flex flex-row items-center">
        <div class="w-full">1</div>
        <div class="w-full flex flex-row justify-center space-x-2">
            <div class="w-auto">
                <Button class="text-muted-foreground hover:bg-primary/0 focus:bg-primary/0 focus-visible:bg-primary/0" variant="outline" size="sm">
                    <PlayIcon class="w-3 h-3" />
                </Button>
            </div>
            <div class="w-auto">
                <Button class="text-muted-foreground" variant="outline" size="default">
                    <PauseIcon class="w-4 h-4" />
                </Button>
            </div>
            <div class="w-auto">
                <Button class="text-muted-foreground" variant="outline" size="sm">
                    <OctagonMinusIcon class="w-3 h-3" />
                </Button>
            </div>
        </div>
        <div class="w-full flex flex-row justify-end space-x-2 pr-2">
            <div class="w-auto">
                <Button class="text-muted-foreground hover:bg-primary/0 focus:bg-primary/0 focus-visible:bg-primary/0" variant="outline" size="sm">
                    <PowerIcon class="w-3 h-3 mr-2" />
                    <span>急停</span>
                </Button>
            </div>
            <div class="w-auto">
                <Button class="text-muted-foreground" variant="outline" size="default">
                    <PlayIcon class="w-4 h-4 mr-2" />
                    <span>启动</span>
                </Button>
            </div>
            <div class="w-auto">
                <Button class="text-muted-foreground" variant="outline" size="sm">
                    <MapPinOffIcon class="w-3 h-3 mr-2" />
                    <span>忽略限位</span>
                </Button>
            </div>
        </div>
    </div>
</template>

<script setup lang="ts">
import {nextTick, onBeforeMount, onBeforeUnmount, onMounted, onUnmounted} from "vue";
import {Button} from "../../packages/york";
import {MapPinOffIcon, PlayIcon, PowerIcon, PauseIcon, OctagonMinusIcon} from "lucide-vue-next";

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
