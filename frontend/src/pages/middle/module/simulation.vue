<template>
    <div class="w-full h-full relative">
        <div id="world" class="w-full h-full"></div>
        <div class="w-full absolute left-0 right-0 bottom-2 flex flex-row items-end text-muted-foreground/80 text-xs px-2">
            <div class="w-full leading-5">
                <div class="dro-line">X 长度: <span>{{props.data.middle.index.simulation.box.x}}</span></div>
                <div class="dro-line">Y 长度: <span>{{props.data.middle.index.simulation.box.y}}</span></div>
                <div class="dro-line">Z 最大值: <span>{{props.data.middle.index.simulation.box.z_max}}</span></div>
                <div class="dro-line">Z 最小值: <span>{{props.data.middle.index.simulation.box.z_min}}</span></div>
                <div class="dro-line">刀具号: <span>{{props.data.middle.index.simulation.tool.id}}</span></div>
                <div class="dro-line">刀具直径: <span>{{props.data.middle.index.simulation.tool.diameter}}</span></div>
                <div class="dro-line">偏移量: <span>{{props.data.middle.index.simulation.tool.offset.toFixed(3)}}</span></div>
                <div class="dro-line">预计时长: <span>0</span></div>
                <div class="dro-line">剩余时长: <span>-</span></div>
            </div>
            <div class="w-full leading-5">
                <div class="dro-line">速度: <span>0.000</span></div>
                <div class="dro-line">X 速度: <span>0.000</span></div>
                <div class="dro-line">Y 速度: <span>0.000</span></div>
                <div class="dro-line">Z 速度: <span>0.000</span></div>
                <div class="dro-line">B 速度: <span>0.000</span></div>
                <div class="dro-line">C 速度: <span>0.000</span></div>
            </div>
            <div class="w-full leading-5">
                <div class="dro-line">G54 X: <span>0.000</span></div>
                <div class="dro-line">G54 Y: <span>0.000</span></div>
                <div class="dro-line">G54 Z: <span>0.000</span></div>
                <div class="dro-line">G54 B: <span>0.000</span></div>
                <div class="dro-line">G54 C: <span>0.000</span></div>
            </div>
            <div class="w-full leading-5">
                <div class="dro-line">G54 X: <span>0.000</span></div>
                <div class="dro-line">G54 Y: <span>0.000</span></div>
                <div class="dro-line">G54 Z: <span>0.000</span></div>
                <div class="dro-line">G54 B: <span>0.000</span></div>
                <div class="dro-line">G54 C: <span>0.000</span></div>
            </div>
            <div class="w-full leading-5">
                <div class="dro-line">G92 X: <span>0.000</span></div>
                <div class="dro-line">G92 Y: <span>0.000</span></div>
                <div class="dro-line">G92 Z: <span>0.000</span></div>
                <div class="dro-line">G92 B: <span>0.000</span></div>
                <div class="dro-line">G92 C: <span>0.000</span></div>
            </div>
            <div class="w-full leading-5">
                <div class="dro-line">DTG X: <span>0.000</span></div>
                <div class="dro-line">DTG Y: <span>0.000</span></div>
                <div class="dro-line">DTG Z: <span>0.000</span></div>
                <div class="dro-line">DTG B: <span>0.000</span></div>
                <div class="dro-line">DTG C: <span>0.000</span></div>
            </div>
        </div>
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
