<template>
    <div class="w-full h-full relative">
        <div id="world" class="w-full h-full"></div>
        <div class="w-[95%] absolute left-0 right-0 bottom-2 flex flex-row items-end text-muted-foreground/80 text-xs px-3" style="pointer-events: none;">
            <div class="w-full leading-5">
                <div class="dro-line">X 长度: <span>{{props.data.machine.box.x}}</span></div>
                <div class="dro-line">Y 长度: <span>{{props.data.machine.box.y}}</span></div>
                <div class="dro-line">Z 最大值: <span>{{props.data.machine.box.z_max}}</span></div>
                <div class="dro-line">Z 最小值: <span>{{props.data.machine.box.z_min}}</span></div>
                <div class="dro-line">刀具号: <span>{{props.data.machine.tool.id}}</span></div>
                <div class="dro-line">刀具直径: <span>{{props.data.machine.tool.diameter}}</span></div>
                <div class="dro-line">偏移量: <span>{{props.data.machine.tool.offset.toFixed(3)}}</span></div>
                <div class="dro-line">预计时长: <span>0</span></div>
                <div class="dro-line">剩余时长: <span>-</span></div>
            </div>
            <div class="w-full leading-5">
                <div class="dro-line">速度: <span>{{props.data.machine.current_velocity.toFixed(3)}}</span></div>
                <div class="dro-line" v-for="(item, index) in props.data.machine.axes" :key="index">{{item.name}} 速度: <span>{{(props.data.machine.velocity[index] * 60).toFixed(3)}}</span></div>
            </div>
            <div class="w-full leading-5">
                <div class="dro-line" v-for="(item, index) in props.data.machine.axes" :key="index">{{props.data.machine.offset.options[props.data.machine.offset.index - 1].name}} {{item.name}}: <span>{{parseFloat(props.data.machine.g5x_offset[index]).toFixed(3)}}</span></div>
            </div>
            <div class="w-full leading-5">
                <div class="dro-line" v-for="(item, index) in props.data.machine.axes" :key="index">{{props.data.machine.offset.options[props.data.machine.offset.index - 1].name}} {{item.name}}: <span>{{parseFloat(props.data.machine.g_offset[index]).toFixed(3)}}</span></div>
            </div>
            <div class="w-full leading-5">
                <div class="dro-line" v-for="(item, index) in props.data.machine.axes" :key="index">G92 {{item.name}}: <span>{{parseFloat(props.data.machine.g92_offset[index]).toFixed(3)}}</span></div>
            </div>
            <div class="w-full leading-5">
                <div class="dro-line" v-for="(item, index) in props.data.machine.axes" :key="index">DTG {{item.name}}: <span>{{parseFloat(props.data.machine.dtg_offset[index]).toFixed(3)}}</span></div>
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
        (window as any).simulation.onLoadCode(props.data.program.lines);
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
