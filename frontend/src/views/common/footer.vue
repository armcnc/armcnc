<template>
    <div class="w-full h-full flex flex-row items-center px-2 text-xs text-muted-foreground/60">
        <div class="w-full flex flex-row items-center space-x-3">
            <div class="w-auto flex flex-row items-center">
                <FileCogIcon class="w-3 h-3 mr-1" />
                <span>{{props.data.machine.path}}</span>
            </div>
            <div class="w-auto flex flex-row items-center">
                <ListCheckIcon class="w-3 h-3 mr-1" />
                <span>{{props.data.machine.program.line + 1}}/{{props.data.program.lines.length}}</span>
            </div>
        </div>
        <div class="w-full"></div>
        <div class="w-full flex flex-row items-center justify-end space-x-3">
            <div class="w-auto flex flex-row items-center">{{ props.data.machine.time}}</div>
            <div class="w-auto flex flex-row items-center">
                <MessageSquareMoreIcon class="w-3 h-3 mr-1" />
                <span>{{props.data.backend.socket.status ? "已连接" : "未连接"}}</span>
            </div>
            <div class="w-auto flex flex-row items-center">
                <MonitorIcon class="w-3 h-3 mr-1" />
                <span>{{props.data.platform}}</span>
            </div>
            <div class="w-auto flex flex-row items-center">
                <ShieldIcon class="w-3 h-3 mr-1" />
                <span>{{props.data.version}}{{props.data.mode ? " D" : ""}}</span>
            </div>
        </div>
    </div>
</template>

<script setup lang="ts">
import {nextTick, onBeforeMount, onBeforeUnmount, onMounted, onUnmounted} from "vue";
import {ListCheckIcon, MessageSquareMoreIcon, MonitorIcon, ShieldIcon, FileCogIcon} from "lucide-vue-next";

const props: any = defineProps<{
    data: any
}>();

onBeforeMount(() => {});

onMounted(() => {
    nextTick(() => {
        console.log("[pages:common:footer]", props);
    });
});

onBeforeUnmount(() => {});

onUnmounted(() => {});
</script>
