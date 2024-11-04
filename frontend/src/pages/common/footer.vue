<template>
    <div class="w-full h-full flex flex-row items-center px-2 text-xs text-muted-foreground/60">
        <div class="w-full text-left">1</div>
        <div class="w-full">2</div>
        <div class="w-full text-right">
            {{props.data.platform}} {{props.data.screen.width + " " + props.data.screen.height}}
        </div>
    </div>
</template>

<script setup lang="ts">
import {nextTick, onBeforeMount, onBeforeUnmount, onMounted, onUnmounted} from "vue";

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
