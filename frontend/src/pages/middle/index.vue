<template>
    <div class="w-full h-full flex flex-row">
        <div class="w-[calc(100%-400px)] flex flex-col">
            <div class="w-full h-[calc(100%-300px)]"></div>
            <div class="w-full h-[300px] border-t border-zinc-900">
                <div class="w-full h-[45px] bg-muted/20 border-b border-zinc-900">1</div>
                <div class="w-full h-[calc(100%-45px)]">2</div>
            </div>
        </div>
        <div class="w-[400px] border-l border-zinc-900">
            <div class="w-full h-full overflow-y-auto p-2">
                <div class="w-full text-sm text-muted-foreground space-y-3">
                    <div class="w-full bg-violet-700/10 border border-violet-900/50 rounded-md">
                        <Table>
                            <TableCaption class="hidden"></TableCaption>
                            <TableHeader class="bg-violet-700/10 border border-violet-900/50 border-none">
                                <TableRow class="border-none text-sm">
                                    <TableHead class="h-9 w-[50px] text-center">轴</TableHead>
                                    <TableHead class="h-9 w-[100px] text-center">坐标(mm)</TableHead>
                                    <TableHead class="h-9 w-[80px] text-center">回零</TableHead>
                                    <TableHead class="h-9 w-[100px] text-center">原点偏移</TableHead>
                                </TableRow>
                            </TableHeader>
                            <TableBody class="text-base">
                                <TableRow class="border-none hover:bg-violet-700/5">
                                    <TableCell class="h-10 text-center">X</TableCell>
                                    <TableCell class="h-10 text-center">-33.462</TableCell>
                                    <TableCell class="h-10">
                                        <MapPinIcon class="w-5 h-5 mx-auto" />
                                    </TableCell>
                                    <TableCell class="h-10 text-center">
                                        <span>0.000</span>
                                    </TableCell>
                                </TableRow>
                                <TableRow class="border-none hover:bg-violet-700/5">
                                    <TableCell class="h-10 text-center">Y</TableCell>
                                    <TableCell class="h-10 text-center">-0.037</TableCell>
                                    <TableCell class="h-10">
                                        <MapPinIcon class="w-5 h-5 mx-auto" />
                                    </TableCell>
                                    <TableCell class="h-10 text-center">
                                        <span>0.000</span>
                                    </TableCell>
                                </TableRow>
                                <TableRow class="border-none hover:bg-violet-700/5">
                                    <TableCell class="text-center">Z</TableCell>
                                    <TableCell class="h-10 text-center">0.273</TableCell>
                                    <TableCell class="h-10">
                                        <MapPinIcon class="w-5 h-5 mx-auto" />
                                    </TableCell>
                                    <TableCell class="h-10 text-center">
                                        <span>0.000</span>
                                    </TableCell>
                                </TableRow>
                            </TableBody>
                        </Table>
                    </div>
                    <div class="w-full"></div>
                </div>
            </div>
        </div>
    </div>
</template>

<script setup lang="ts">
import {nextTick, onBeforeMount, onBeforeUnmount, onMounted, onUnmounted} from "vue";
import {Input} from "../../packages/york";
import {Table, TableBody, TableCell, TableHead, TableHeader, TableRow, TableCaption} from "../../packages/york";
import {MapPinIcon} from "lucide-vue-next";

const props: any = defineProps<{
    data: any
}>();

onBeforeMount(() => {});

onMounted(() => {
    nextTick(() => {
        console.log("[pages:middle:index]", props);
    });
});

onBeforeUnmount(() => {});

onUnmounted(() => {});
</script>
