import { defineConfig } from "vite";
import vue from "@vitejs/plugin-vue";
import tailwind from "tailwindcss";
import autoprefixer from "autoprefixer";
import Package from "./package.json";

const timestamp = (length: number)=>{
	const characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	let result = '';
	const charactersLength = characters.length;
	for (let i = 0; i < length; i++) {
		result += characters.charAt(Math.floor(Math.random() * charactersLength));
	}
	return result;
};

export default defineConfig(({mode}) => ({
	root: __dirname,
	define: {
		__VUE_PROD_HYDRATION_MISMATCH_DETAILS__: "true",
		__APP_VERSION__: JSON.stringify(Package.version),
	},
	esbuild: {
		drop: mode === "production" ? ["console", "debugger"] : [],
	},
	plugins: [
		vue(
			{
				template: {
					compilerOptions: {
						isCustomElement: (tag: any) => ["webview"].includes(tag),
					}
				}
			}
		),
	],
	server: {
		host: Package.env.VITE_DEV_SERVER_HOST,
		port: Package.env.VITE_DEV_SERVER_PORT,
		proxy: {
			"/backend": {
				target: "http://192.168.31.58:1081",
				secure: false,
				changeOrigin: true,
				rewrite: (path: any) => path.replace(/^\/backend/, '')
			},
		}
	},
	css: {
		postcss: {
			plugins: [tailwind(), autoprefixer()],
		}
	},
	build: {
		outDir: "./release",
		emptyOutDir: true,
		sourcemap: false,
		cssCodeSplit: true,
		terserOptions: {
			compress: {
				drop_console: true,
				drop_debugger: true,
			},
		},
		rollupOptions: {
			treeshake: true,
			output: {
				entryFileNames: `index/[name]-${Package.version}-${timestamp(16)}.js`,
				chunkFileNames: `chunks/[hash]-${Package.version}-${timestamp(16)}.js`,
				assetFileNames: `assets/[hash]-${Package.version}-${timestamp(16)}.[ext]`,
				manualChunks(id) {
					if (id.includes("node_modules")) {
						const dependenciesKeys = Object.keys(Package.dependencies);
						const match = dependenciesKeys.find((item) => {
							return id.includes(item);
						});
						const notSplit = ["vue"];
						if (match && !notSplit.includes(match)) {
							return match;
						}
					}
				}
			}
		},
		commonjsOptions: {
			requireReturnsDefault: "namespace",
		}
	},
	resolve: {
		alias: {}
	}
}));

