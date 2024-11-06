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
        if(message.type === "init:engine:complete"){
            openProgram();
        }
    });
    (window as any).simulation.InitEngine();
}

function openProgram(){
    if((window as any).simulation){
        (window as any).simulation.clearToolLine();
    }
}

onBeforeMount(() => {});

onMounted(() => {
    nextTick(() => {
        console.log("[pages:middle:module:simulation]", props);
        setTimeout(()=>{
            onSimulation();
        }, 2000);
    });
});

onBeforeUnmount(() => {
    if((window as any).simulation){
        (window as any).simulation.onEngineDestroy();
        (window as any).simulation = false;
    }
});

onUnmounted(() => {});
</script>
