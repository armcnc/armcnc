<template>
    <router-view ref="routerView" v-slot="{Component}">
        <component :is="Component" :data="data" />
    </router-view>
    <Toaster ref="toaster"></Toaster>
</template>

<script setup lang="ts">
import {onBeforeMount, onMounted, onBeforeUnmount, onUnmounted, nextTick, ref} from "vue";
import {useRoute, useRouter} from "vue-router";
import {Request, useI18n, Format, GetPlatform} from "./packages";
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
        toast: toast
    },
    version: "-",
    platform: GetPlatform(),
    screen: {
        width: "-",
        height: "-"
    },
    backend: {
        status: false
    },
    middle: {
        current: "index"
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
