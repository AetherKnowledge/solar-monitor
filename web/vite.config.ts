import adapter from '@sveltejs/adapter-static';
import { sveltekit } from '@sveltejs/kit/vite';
import tailwindcss from '@tailwindcss/vite';
import 'dotenv/config';
import { defineConfig, loadEnv, type Plugin as VitePlugin } from 'vite';
import version from '../version.json';
import { gzipOnly } from './plugins/gzip-only';

const websitePath = '../firmware/data';

function websiteVersion(): VitePlugin {
	return {
		name: 'website-version',

		generateBundle() {
			this.emitFile({
				type: 'asset',
				fileName: 'version.json',
				source: JSON.stringify({
					version: version.website
				})
			});
		}
	};
}

export default defineConfig(({ command, mode }) => {
	const env = loadEnv(mode, '..', '');

	return {
		define: {
			__WEBSITE_VERSION__: JSON.stringify(version.website)
		},
		plugins: [
			tailwindcss(),
			sveltekit({
				compilerOptions: {
					// Force runes mode for the project, except for libraries. Can be removed in svelte 6.
					runes: ({ filename }) =>
						filename.split(/[/\\]/).includes('node_modules') ? undefined : true,
					experimental: {
						async: true
					}
				},

				// adapter-auto only supports some environments, see https://svelte.dev/docs/kit/adapter-auto for a list.
				// If your environment is not supported, or you settled on a specific environment, switch out the adapter.
				// See https://svelte.dev/docs/kit/adapters for more information about adapters.
				adapter: adapter({
					pages: websitePath,
					assets: websitePath,
					fallback: 'index.html'
				})
			}),
			websiteVersion(),
			gzipOnly(websitePath)
		],
		build: {
			sourcemap: false,
			rollupOptions: {
				output: {
					manualChunks: () => 'app'
				}
			}
		},
		server:
			command === 'serve'
				? {
						proxy: {
							'/api': {
								target: `http://${env.DEVICE_IP}`,
								changeOrigin: true,
								ws: true
							},
							'/test': {
								target: 'http://localhost:3001',
								changeOrigin: true
							}
						}
					}
				: undefined
	};
});
