<template>
    <div class="w-full h-full flex flex-row">
        <div class="w-[calc(100%-400px)] flex flex-col">
            <div class="w-full h-[calc(100%-300px)]">
                <div class="w-full h-full">1</div>
            </div>
            <div class="w-full h-[300px] border-t border-zinc-900">
                <div class="w-full h-[45px] border-b border-zinc-900 px-2">
                    <div class="w-full h-full flex flex-row items-center">
                        <div class="w-full flex flex-row items-center space-x-2">
                            <div class="w-auto">
                                <Button class="w-auto px-2 text-muted-foreground" variant="outline" size="sm">
                                    <FileCogIcon class="w-4 h-4 mr-1" />
                                    <span class="min-w-[120px] line-clamp-1 text-left">default_ethercat_sim</span>
                                </Button>
                            </div>
                            <div class="w-auto">
                                <Button class="w-auto px-2 text-muted-foreground" variant="outline" size="sm">
                                    <RefreshCwIcon class="w-4 h-4" />
                                </Button>
                            </div>
                        </div>
                        <div class="w-full flex flex-row items-center justify-center space-x-2">
                            <Button class="w-auto px-3 text-muted-foreground" variant="outline" size="sm">
                                <span>X</span>
                            </Button>
                            <Button class="w-auto px-3 text-muted-foreground" variant="outline" size="sm">
                                <span>Y</span>
                            </Button>
                            <Button class="w-auto px-3 text-muted-foreground" variant="outline" size="sm">
                                <span>Z</span>
                            </Button>
                            <Button class="w-auto px-3 text-muted-foreground" variant="outline" size="sm">
                                <span>P</span>
                            </Button>
                        </div>
                        <div class="w-full flex flex-row items-center justify-end space-x-2">
                            <div class="w-auto">
                                <Button class="w-auto px-2 text-muted-foreground" variant="outline" size="sm">
                                    <ListEndIcon class="w-4 h-4" />
                                </Button>
                            </div>
                            <div class="w-auto">
                                <Button class="w-auto px-2 text-muted-foreground" variant="outline" size="sm">
                                    <ListVideoIcon class="w-4 h-4" />
                                </Button>
                            </div>
                        </div>
                    </div>
                </div>
                <div class="w-full h-[calc(100%-45px)] overflow-y-auto">
                    <div class="w-full p-1 text-muted-foreground/50">
                        <div class="w-full h-6 flex flex-row items-center text-sm text-muted-foreground">
                            <div class="w-full">G1 F1000 X6.7039 Y9.0616 Z-1.0000 S1200</div>
                            <div class="w-auto text-xs">1200</div>
                        </div>
                        <div class="w-full h-6 flex flex-row items-center text-sm">
                            <div class="w-full">G1 F1000 X10.4246 Y19.4013 Z-1.0000 S1200</div>
                            <div class="w-auto text-xs">1201</div>
                        </div>
                        <div class="w-full h-6 flex flex-row items-center text-sm">
                            <div class="w-full">G1 F1000 X10.4246 Y19.4013 Z-1.0000 S1200</div>
                            <div class="w-auto text-xs">1202</div>
                        </div>
                        <div class="w-full h-6 flex flex-row items-center text-sm">
                            <div class="w-full">G1 F300 Z-2.0000</div>
                            <div class="w-auto text-xs">1203</div>
                        </div>
                        <div class="w-full h-6 flex flex-row items-center text-sm">
                            <div class="w-full">G1 F1000 X6.7039 Y9.0616 Z-2.0000 S1200</div>
                            <div class="w-auto text-xs">1204</div>
                        </div>
                        <div class="w-full h-6 flex flex-row items-center text-sm">
                            <div class="w-full">G1 F1000 X14.2106 Y9.0616 Z-2.0000 S1200</div>
                            <div class="w-auto text-xs">1205</div>
                        </div>
                        <div class="w-full h-6 flex flex-row items-center text-sm">
                            <div class="w-full">G1 F1000 X10.4246 Y19.4013 Z-2.0000 S1200</div>
                            <div class="w-auto text-xs">1206</div>
                        </div>
                        <div class="w-full h-6 flex flex-row items-center text-sm">
                            <div class="w-full">G1 F1000 X10.4246 Y19.4013 Z-2.0000 S1200</div>
                            <div class="w-auto text-xs">1207</div>
                        </div>
                        <div class="w-full h-6 flex flex-row items-center text-sm">
                            <div class="w-full"></div>
                            <div class="w-auto text-xs">1208</div>
                        </div>
                        <div class="w-full h-6 flex flex-row items-center text-sm">
                            <div class="w-full">G0 Z3</div>
                            <div class="w-auto text-xs">1209</div>
                        </div>
                        <div class="w-full h-6 flex flex-row items-center text-sm">
                            <div class="w-full">G0 X24.5894 Y19.3882</div>
                            <div class="w-auto text-xs">1210</div>
                        </div>
                    </div>
                </div>
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
                                    <TableCell class="h-12 text-center">X</TableCell>
                                    <TableCell class="h-12 text-center">-33.462</TableCell>
                                    <TableCell class="h-12">
                                        <MapPinIcon class="w-5 h-5 mx-auto" />
                                    </TableCell>
                                    <TableCell class="h-12 text-center p-1">
                                        <div class="w-full h-8 rounded-md border border-violet-900/20 leading-8 text-center text-sm">0.000</div>
                                    </TableCell>
                                </TableRow>
                                <TableRow class="border-none hover:bg-violet-700/5">
                                    <TableCell class="h-12 text-center">Y</TableCell>
                                    <TableCell class="h-12 text-center">-0.037</TableCell>
                                    <TableCell class="h-12">
                                        <MapPinIcon class="w-5 h-5 mx-auto" />
                                    </TableCell>
                                    <TableCell class="h-12 text-center p-1">
                                        <div class="w-full h-8 rounded-md border border-violet-900/20 leading-8 text-center text-sm">0.000</div>
                                    </TableCell>
                                </TableRow>
                                <TableRow class="border-none hover:bg-violet-700/5">
                                    <TableCell class="text-center">Z</TableCell>
                                    <TableCell class="h-12 text-center">0.273</TableCell>
                                    <TableCell class="h-12">
                                        <MapPinIcon class="w-5 h-5 mx-auto" />
                                    </TableCell>
                                    <TableCell class="h-12 text-center p-1">
                                        <div class="w-full h-8 rounded-md border border-violet-900/20 leading-8 text-center text-sm">0.000</div>
                                    </TableCell>
                                </TableRow>
                            </TableBody>
                        </Table>
                    </div>
                    <div class="w-full bg-zinc-900/50 rounded-md p-2">
                        <div class="w-full flex flex-row items-center space-x-2">
                            <div class="w-full">
                                <Button class="w-full px-3 text-muted-foreground" variant="outline" size="lg">
                                    <MapPinIcon class="w-4 h-4 mr-2 text-orange-500" />
                                    <span>全部回零</span>
                                </Button>
                            </div>
                            <div class="w-full">
                                <Button class="w-full px-3 text-muted-foreground" variant="outline" size="lg">
                                    <span>P1 G54</span>
                                </Button>
                            </div>
                            <div class="w-full">
                                <Button class="w-full px-3 text-muted-foreground" variant="outline" size="lg">
                                    <MapPinnedIcon class="w-4 h-4 mr-2 text-orange-500" />
                                    <span>设置原点</span>
                                </Button>
                            </div>
                        </div>
                    </div>
                    <div class="w-full p-2">

                    </div>
                    <div class="w-full bg-zinc-900/50 rounded-md p-2 space-y-1">
                        <div class="w-full h-8 leading-8 text-muted-foreground/50">步进长度(mm)</div>
                        <div class="w-full grid grid-cols-6 gap-2 text-center text-sm">
                            <div class="w-auto h-9 leading-9 bg-zinc-950/40 rounded-md px-3 relative">10</div>
                            <div class="w-auto h-9 leading-9 bg-zinc-950/40 rounded-md px-3 relative">5</div>
                            <div class="w-auto h-9 leading-9 bg-zinc-950/40 rounded-md px-3 relative">1</div>
                            <div class="w-auto h-9 leading-9 bg-zinc-950/40 rounded-md px-3 relative">0.5</div>
                            <div class="w-auto h-9 leading-9 bg-zinc-950/40 rounded-md px-3 relative">0.1</div>
                            <div class="w-auto h-9 leading-9 bg-zinc-950/40 rounded-md px-3 relative">0.05</div>
                            <div class="w-auto h-9 leading-9 bg-zinc-950/40 rounded-md px-3 relative">0.01</div>
                            <div class="w-auto h-9 leading-9 bg-zinc-950/40 rounded-md px-3 relative">0.005</div>
                            <div class="w-auto h-9 leading-9 bg-zinc-950 rounded-md px-3 relative">
                                <span>连续</span>
                                <span class="absolute w-[60%] rounded-md h-[2px] bg-primary left-0 right-0 bottom-1 mx-auto"></span>
                            </div>
                        </div>
                    </div>
                    <div class="w-full p-2">1</div>
                </div>
            </div>
        </div>
    </div>
</template>

<script setup lang="ts">
import {nextTick, onBeforeMount, onBeforeUnmount, onMounted, onUnmounted} from "vue";
import {Button} from "../../packages/york";
import {Table, TableBody, TableCell, TableHead, TableHeader, TableRow, TableCaption} from "../../packages/york";
import {MapPinIcon, MapPinnedIcon, FileCogIcon, RefreshCwIcon, ListEndIcon, ListVideoIcon} from "lucide-vue-next";

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
