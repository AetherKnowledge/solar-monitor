import adapter from '@sveltejs/adapter-static';
import { sveltekit } from '@sveltejs/kit/vite';
import tailwindcss from '@tailwindcss/vite';
import { defineConfig } from 'vite';
import { gzipOnly } from './plugins/gzip-only';

const websitePath = '../firmware/data';

export default defineConfig(({ command }) => ({
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
							target: 'http://192.168.254.122',
							changeOrigin: true
						},
						'/test': {
							target: 'http://localhost:3001',
							changeOrigin: true
						}
					}
				}
			: undefined
}));
