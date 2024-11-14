<template>
    <router-view ref="routerView" v-slot="{Component}">
        <component :is="Component" :data="data" />
    </router-view>
    <Toaster ref="toaster"></Toaster>
</template>

<script setup lang="ts">
import {onBeforeMount, onMounted, onBeforeUnmount, onUnmounted, nextTick, ref} from "vue";
import {useRoute, useRouter} from "vue-router";
import {Request, useI18n, Format, GetPlatform, Simulation} from "./packages";
import {Toaster, useToast} from "./packages/york";

const i18n = useI18n();
const $route = useRoute();
const $router = useRouter();
const {toast} = useToast();

const data: any = ref({
    route: $route,
    router: $router,
    tools: {
        language: {
            current: (navigator as any).language,
            locale: i18n.locale,
            t: i18n.t,
            f: Format
        },
        request: Request,
        simulation: Simulation,
        toast: toast
    },
    mode: import.meta.env.DEV,
    version: "-",
    platform: GetPlatform(),
    screen: {
        width: "-",
        height: "-"
    },
    backend: {
        status: false,
        socket: {
            status: false,
            connect: false,
        },
    },
    middle: {
        current: "dashboard",
        index: {
            simulation: {
                current_velocity: 0,
                velocity: [],
                g92_offset: [],
                g5x_offset: [],
                g_offset: [],
                dtg_offset: [],
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
                time: Math.floor(Date.now() / 1000)
            }
        }
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
