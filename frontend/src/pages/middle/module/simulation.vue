<template>
    <div class="w-full h-full">
        <div id="world" class="w-full h-full"></div>
    </div>
</template>

<script setup lang="ts">
import {nextTick, onBeforeMount, onBeforeUnmount, onMounted, onUnmounted} from "vue";

const props: any = defineProps<{
    data: any
}>();

function onSimulation(){
    let world: any = document.getElementById("world");
    world.innerHTML = "";
    (window as any).simulation = new props.data.tools.simulation(world, props.data.platform, (message: any)=>{
        console.log(message);
    });
    (window as any).simulation.InitEngine();
}

onBeforeMount(() => {});

onMounted(() => {
    nextTick(() => {
        console.log("[pages:middle:module:simulation]", props);
        onSimulation();
    });
});

onBeforeUnmount(() => {});

onUnmounted(() => {});
</script>
