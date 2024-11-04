<template>
    <div class="w-full h-[35px] flex flex-row items-center px-2 text-xs text-muted-foreground">
        <div class="w-full text-left">1</div>
        <div class="w-full">2</div>
        <div class="w-full text-right">3</div>
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
