<template>
    <router-view ref="routerView" v-slot="{Component}">
        <component :is="Component" :data="data" />
    </router-view>
    <Toaster ref="toaster"></Toaster>
</template>

<script setup lang="ts">
import { nextTick, onBeforeMount, onBeforeUnmount, onMounted, onUnmounted, ref } from "vue";
import { Simulation } from "./packages";
import { Toaster, useToast } from "./packages/york";
import { useBaseStore, useLanguageStore, useRequestStore } from "./stores";

const base = useBaseStore();
const language = useLanguageStore();
const request = useRequestStore();
const {toast} = useToast();

const data: any = ref({
    base: {
        route: base.$useRoute,
        router: base.$useRouter
    },
    tools: {
        language: {
            current: (navigator as any).language,
            locale: language.$I18n.useI18n().locale,
            t: language.$I18n.useI18n().t,
            f: language.$Format
        },
        request: request.$Request,
        simulation: Simulation,
        toast: toast
    },
    mode: import.meta.env.DEV,
    version: "-",
    platform: base.$GetPlatform(),
    backend: {
        status: false,
        socket: {
            status: false,
            connect: false,
        },
    },
    current: "dashboard",
    program: {
        data: "",
        lines: []
    },
    machine: {
        path: "-",
        file: "",
        data: false,
        is_first: true,
        axes: [],
        home: "",
        homed: false,
        relative_offset: "",
        linear_units: "-",
        angular_units: "-",
        current_velocity: 0,
        velocity: [],
        g92_offset: [],
        g5x_offset: [],
        g_offset: [],
        dtg_offset: [],
        program: {
            line: 0
        },
        box: {
            x: 0,
            y: 0,
            z_min: 0,
            z_max: 0
        },
        tool: {
            id: 0,
            offset: 0.000,
            diameter: 0
        },
        offset: {
            index: 0,
            options: [
                {
                    label: "坐标系",
                    value: 0,
                    name: "",
                    p_name: ""
                }
            ]
        },
        spindle: {
            enabled: 0,
            direction: 0,
            min_velocity: 0,
            max_velocity: 0,
            default_speed: 0,
            speed: 0,
            min_override: 0,
            max_override: 0,
            override: 100
        },
        feed: {
            override: 100,
            max_override: 0
        },
        max_linear_velocity: 0,
        max_velocity: 0,
        default_linear_velocity: 0,
        max_angular_velocity: 0,
        default_angular_velocity: 0,
        rocker: {
            status: false,
            items: [
            {
                name: "",
                value: "",
                arrow: "",
                hide: true,
            },
            {
                name: "",
                value: "",
                arrow: "up",
                hide: true,
            },
            {
                name: "",
                value: "",
                arrow: "",
                hide: true,
            },
            {
                name: "",
                value: "",
                arrow: "up",
                hide: true,
            },
            {
                name: "",
                value: "",
                arrow: "left",
                hide: true,
            },
            {
                name: "",
                value: "",
                arrow: "down",
                hide: true,
            },
            {
                name: "",
                value: "",
                arrow: "right",
                hide: true,
            },
            {
                name: "",
                value: "",
                arrow: "down",
                hide: true,
            },
            {
                name: "",
                value: "",
                arrow: "left",
                hide: true,
            },
            {
                name: "",
                value: "",
                arrow: "right",
                hide: true,
            },
            {
                name: "",
                value: "",
                arrow: "left",
                hide: true,
            },
            {
                name: "",
                value: "",
                arrow: "right",
                hide: true,
            }
            ]
        },
        step: {
            value: -1,
            items: [
                {
                    label: "10",
                    value: 10
                },
                {
                    label: "5",
                    value: 5
                },
                {
                    label: "1",
                    value: 1
                },
                {
                    label: "0.5",
                    value: 0.5
                },
                {
                    label: "0.1",
                    value: 0.1
                },
                {
                    label: "0.05",
                    value: 0.05
                },
                {
                    label: "0.01",
                    value: 0.01
                }
            ]
        },
        state: {
            start: "disabled",
            start_disabled: true,
            pause: "disabled",
            pause_disabled: true,
            stop: "disabled",
            stop_disabled: true,
            estop: "",
            enabled: "",
            limit: ""
        },
        time: Math.floor(Date.now() / 1000)
    }
});

onBeforeMount(() => {});

onMounted(() => {
    nextTick(() => {
        console.log("[app]", data.value);
        if(data.value.tools.language.locale === "null"){
            if(data.value.tools.language.current === "zh-CN"){
                data.value.tools.language.locale = "en";
            }else{
                data.value.tools.language.locale = "en";
            }
        }
    });
});

onBeforeUnmount(() => {});

onUnmounted(() => {});
</script>

<style>
@import url("./assets/css/font.css");
@import url("./assets/css/base.css");
@import url("./assets/css/theme.css");
@import url("./assets/css/animation.css");
@import url("./assets/css/keyframe.css");
</style>
