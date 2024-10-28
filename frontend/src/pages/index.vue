<template>
    <div class="grid gap-0 grid-cols-1 fixed w-full h-full bg-zinc-950 z-[50000]" v-if="!props.data.backend.status">
        <div class="w-5 h-5 m-auto items-center">
            <svg class="animate-spin h-5 w-5" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24">
                <circle class="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" stroke-width="4"></circle>
                <path class="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4zm2 5.291A7.962 7.962 0 014 12H0c0 3.042 1.135 5.824 3 7.938l3-2.647z"></path>
            </svg>
        </div>
    </div>
    <div class="relative flex w-full flex-col items-center" v-else>
        <div class="w-full md:h-[550px] h-[400px] relative flex w-full flex-col items-center z-1">
            <div class="w-full h-full grid items-center justify-center">
                <div class="w-[350px] text-center p-4 rounded-md border border-zinc-900 space-y-4">
                    <div class="w-full">
                        <InfoCircledIcon class="w-6 h-6 mx-auto text-muted-foreground/60" />
                    </div>
                    <div class="w-full text-muted-foreground/60 text-xs">This feature is not yet open</div>
                </div>
            </div>
        </div>

    </div>
</template>

<script setup lang="ts">
import {nextTick, onBeforeMount, onBeforeUnmount, onMounted, onUnmounted} from "vue";
import {InfoCircledIcon} from "@radix-icons/vue";

const props: any = defineProps<{
    data: any
}>();

function onService(){
    setTimeout(()=>{
        props.data.backend.status = true;
    }, 3000);
}

onBeforeMount(() => {});

onMounted(() => {
    nextTick(() => {
        console.log("[pages:index]", props);
        onService();
    });
});

onBeforeUnmount(() => {});

onUnmounted(() => {});
</script>
