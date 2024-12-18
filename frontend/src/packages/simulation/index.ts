// Copyright 2024 ARMCNC, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// @ts-ignore
import * as THREE from "three";
// @ts-ignore
import {OrbitControls} from "three/examples/jsm/controls/OrbitControls";
// @ts-ignore
import {ViewHelper} from "three/examples/jsm/helpers/ViewHelper";
// @ts-ignore
import {TransformControls} from "three/examples/jsm/controls/TransformControls.js";
import Gcode from "./plugins/gcode";

export class Simulation {

    constructor(world: any, platform: string, loading_callback: any) {
        const _this: any = this;
        _this.platform = platform;
        _this.loading_callback = loading_callback;
        _this.loading = new THREE.LoadingManager(()=>{
            loading_callback({type: "resource:loading:end"});
        }, (url: any, loaded: any, total: any)=>{
            loading_callback({type: "resource:loading:progress", progress: parseFloat(((loaded * 100) / total).toFixed(2))});
        });
        _this.container = world;
        _this.engine = {
            scene: false,
            clock: false,
            light: {
                ambient: false,
                hemisphere: false,
                directional: false
            },
            ground: false,
            axes_helper: false,
            control: false,
            control_camera: false,
            view_helper: false,
            tool: false,
            tool_lines: false,
            renderer: false,
        }
        _this.gocode = new Gcode();
        _this.view = "p";
        _this.object = {
            box: false
        }
    }

    InitEngine(){
        const _this: any = this;

        _this.engine.scene = new THREE.Scene();
        _this.engine.scene.name = "default_scene";
        _this.engine.scene.background = new THREE.Color("rgb(9, 9, 11)");

        _this.engine.clock = new THREE.Clock();
        _this.engine.clock_delta = 0.00;

        _this.engine.ground = new THREE.GridHelper(50, 50, 0xf9f9f9, 0xf9f9f9);
        _this.engine.ground.name = "default_ground";
        _this.engine.ground.material.transparent = true;
        _this.engine.ground.material.opacity = 0.05;
        //_this.engine.ground.rotation.x = 90 * Math.PI / 180;
        _this.engine.ground.applyMatrix4(new THREE.Matrix4().makeRotationX(Math.PI / 2));

        _this.engine.scene.add(_this.engine.ground);

        _this.engine.axes_helper = new THREE.AxesHelper(1, 1, 1);
        _this.engine.axes_helper.name = "default_axes_helper";
        _this.engine.axes_helper.position.y = 0;
        _this.engine.scene.add(_this.engine.axes_helper);

        _this.engine.control_camera = new THREE.PerspectiveCamera(75, _this.container.clientWidth / _this.container.clientHeight, 0.0001, 20000);
        _this.engine.control_camera.name = "default_camera";
        _this.engine.control_camera.position.set(0, -10, 5);
        _this.engine.control_camera.lookAt(0, 0, 0);
        _this.engine.control_camera.up.set(0, 0, 1);
        _this.engine.control = new OrbitControls(_this.engine.control_camera, _this.container);
        _this.engine.control.enabled = true;
        _this.engine.control.mouseButtons.RIGHT = 0; // 禁用右键拖拽
        _this.engine.control.minDistance = 1; // 相机距离观察目标最小距离
        _this.engine.control.maxDistance = 40; // 相机距离观察目标最大距离
        _this.engine.control.maxPolarAngle = Math.PI / 2;
        _this.engine.control.enableDamping = true;
        _this.engine.control.dampingFactor = 0.2; // 视角惯性量
        _this.engine.control.target.set(0, 0, 0);
        _this.engine.control.update();
        _this.engine.scene.add(_this.engine.control_camera);

        _this.engine.view_helper = new ViewHelper(_this.engine.control_camera, _this.container);
        _this.engine.view_helper.scale.set(0.7, 0.7, 0.7);
        _this.engine.view_helper.position.set(0.8, -0.8, -1);

        _this.engine.tool = new THREE.Object3D();
        _this.engine.tool.name = "default_tool";
        _this.engine.tool.applyMatrix4(new THREE.Matrix4().makeRotationX(Math.PI / 2));
        _this.engine.tool.add(new THREE.Mesh(new THREE.CylinderGeometry(0.1, 0, 0.3, 32, 2, false)));
        _this.engine.tool.position.z = 0.25;
        _this.engine.tool.matrixAutoUpdate = true;
        _this.engine.tool_lines = new THREE.Group();
        _this.engine.tool_lines.userData = {
            count: 0
        }
        _this.engine.scene.add(_this.engine.tool, _this.engine.tool_lines);

        _this.engine.renderer = new THREE.WebGLRenderer({antialias: true, alpha: true});
        _this.engine.renderer.setClearColor(0x00FF00, 1);
        _this.engine.renderer.setPixelRatio(window.devicePixelRatio);
        _this.engine.renderer.sortObjects = false;
        _this.engine.renderer.autoClear = false;
        _this.engine.renderer.updateSize = _this.engine.renderer.setSize;
        _this.container.appendChild(_this.engine.renderer.domElement);

        _this.onEngineResize();
        _this.onEngineEvent();
        _this.onEngineAnimate()

        _this.loading_callback({
            type: "init:engine:complete"
        });
    }

    onLoadCode(lines: any){
        const _this: any = this;
        _this.clearGcode();
        _this.gocode.load(lines, (object: any)=>{
            _this.engine.scene.add(object);
            if(object.userData.dimensions){
                _this.object.box = new THREE.Box3().setFromObject(object);
                const center = _this.object.box.getCenter(new THREE.Vector3());
                _this.engine.control.target.set(center.x, center.y, center.z);
                _this.engine.control.update();
                _this.engine.control_camera.lookAt(center);
                _this.view = "p";
                _this.loading_callback({
                    type: "resource:update:data",
                    box: object.userData.dimensions,
                    view: _this.view
                });
            }
        });
    }

    updateToolPosition(state: any, homing: any, x: any, y: any, z: any){
        const _this: any = this;
        const gcode = _this.engine.scene.getObjectByName("gcode");
        if(_this.engine.tool && gcode){
            _this.engine.tool.position.x = x * 0.1;
            _this.engine.tool.position.y = y * 0.1;
            _this.engine.tool.position.z = (z * 0.1) + 0.15;
        }
        if (!homing && gcode) {
            if(state === 2){
                gcode.children.forEach((lineSegment: any) => {
                    if (lineSegment.material && lineSegment.material.isMaterial) {
                        lineSegment.material.transparent = true;
                        lineSegment.material.opacity = 0.2;
                        lineSegment.material.needsUpdate = true;
                    }
                });
                if(_this.engine.tool_lines.children.length === 0 || _this.engine.tool_lines.userData.count === 1000){
                    _this.engine.tool_lines.userData.count = 0;
                    let line = new THREE.Line(new THREE.BufferGeometry(), new THREE.LineBasicMaterial({color: 0x00FF00}));
                    line.name = "tool_line";
                    const newPosition = new THREE.Vector3(_this.engine.tool.position.x, _this.engine.tool.position.y, _this.engine.tool.position.z - 0.15);
                    line.geometry.setAttribute("position", new THREE.Float32BufferAttribute([newPosition.x, newPosition.y, newPosition.z], 3));
                    _this.engine.tool_lines.userData.count++;
                    _this.engine.tool_lines.add(line);
                }else{
                    const newPosition = new THREE.Vector3(_this.engine.tool.position.x, _this.engine.tool.position.y, _this.engine.tool.position.z - 0.15);
                    const vertices = (_this.engine.tool_lines.children[_this.engine.tool_lines.children.length - 1].geometry as THREE.BufferGeometry).attributes.position.array as Float32Array;
                    const newVertices = Float32Array.from([...vertices, newPosition.x, newPosition.y, newPosition.z]);
                    _this.engine.tool_lines.children[_this.engine.tool_lines.children.length - 1].geometry.setAttribute("position", new THREE.Float32BufferAttribute(newVertices, 3));
                    _this.engine.tool_lines.userData.count++;
                }
            }else{
                gcode.children.forEach((lineSegment: any) => {
                    if (lineSegment.material && lineSegment.material.isMaterial) {
                        lineSegment.material.transparent = false;
                        lineSegment.material.needsUpdate = true;
                    }
                });
            }
        }
    }

    clearToolLine(){
        const _this: any = this;
        if(_this.engine.tool_lines.children.length > 0){
            _this.engine.tool_lines.children.forEach((object: any)=>{
                if (object.isLine) {
                    object.geometry.dispose();
                    object.material.dispose();
                }
                _this.engine.tool_lines.remove(object);
            });
        }
    }

    clearGcode(){
        const _this: any = this;
        const gcode = _this.engine.scene.getObjectByName("gcode");
        if(gcode){
            gcode.children.forEach((child: any)=>{
                if (child.isObject3D) {
                    child.geometry.dispose();
                    if (child.material) {
                        child.material.dispose();
                        if (child.material.map) child.material.map.dispose();
                        if (child.material.lightMap) child.material.lightMap.dispose();
                        if (child.material.bumpMap) child.material.bumpMap.dispose();
                        if (child.material.normalMap) child.material.normalMap.dispose();
                        if (child.material.specularMap) child.material.specularMap.dispose();
                        if (child.material.envMap) child.material.envMap.dispose();
                    }
                }
            });
            _this.engine.scene.remove(gcode);
            if (_this.engine.renderer) {
                _this.engine.renderer.renderLists.dispose();
            }
        }
    }

    setView(view: string){
        const _this: any = this;
        const center = _this.object.box.getCenter(new THREE.Vector3());
        const distance = 10;
        const views: any = {
            x: new THREE.Vector3(-1, 0, 0),
            y: new THREE.Vector3(0, -1, 0),
            z: new THREE.Vector3(0, 0, 1),
            p: new THREE.Vector3(0, -10, 5),
        };
        const new_view = views[view];
        if (!new_view){
            return;
        }
        if(view === "p"){
            _this.engine.control_camera.position.set(0, -10, 5);
            _this.engine.control_camera.lookAt(0, 0, 0);
            _this.engine.control_camera.up.set(0, 0, 1);
            _this.engine.control.update();
        }else{
            const newPosition = new_view.clone().multiplyScalar(distance).add(center);
            _this.engine.control_camera.position.copy(newPosition);
            _this.engine.control_camera.lookAt(center);
            _this.engine.control.target.copy(center);
            _this.engine.control.update();
        }
        _this.view = view;
        _this.loading_callback({
            type: "resource:update:data",
            view: _this.view
        });
    }

    onEngineResize(){
        const _this: any = this;
        _this.engine.control_camera.aspect = _this.container.clientWidth / _this.container.clientHeight;
        _this.engine.control_camera.updateProjectionMatrix();
        _this.engine.renderer.updateSize(_this.container.clientWidth, _this.container.clientHeight);
    }

    onEngineEvent(){
        const _this: any = this;
    }

    onEngineAnimate(){
        const _this: any = this;
        _this.engine.clock_delta = _this.engine.clock.getDelta();
        _this.engine.control.update();
        _this.engine.renderer.clear();
        _this.engine.renderer.render(_this.engine.scene, _this.engine.control_camera);
        _this.engine.view_helper.render(_this.engine.renderer);
        requestAnimationFrame(_this.onEngineAnimate.bind(_this));
        _this.onEngineResize();
    }

    onEngineDestroy(){
        const _this: any = this;
        cancelAnimationFrame(_this.onEngineAnimate.bind(_this));
        _this.clearGcode();
        _this.engine.scene.children.forEach((object: any)=>{
            if (object.isMesh) {
                object.geometry.dispose();
                object.material.dispose();
            }
            _this.engine.scene.remove(object);
        });
        _this.engine.renderer.dispose();
    }
}