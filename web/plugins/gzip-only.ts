import fs from 'node:fs';
import path from 'node:path';
import zlib from 'node:zlib';
import type { Plugin } from 'vite';

const COMPRESSIBLE_EXTENSIONS = new Set([
	'.html',
	'.htm',
	'.css',
	'.js',
	'.mjs',
	'.json',
	'.svg',
	'.xml',
	'.txt',
	'.map',
	'.woff2'
]);

export function gzipOnly(outputDir: string): Plugin {
	return {
		name: 'gzip-only',

		closeBundle() {
			function walk(dir: string) {
				for (const entry of fs.readdirSync(dir)) {
					const file = path.join(dir, entry);
					const stat = fs.statSync(file);

					if (stat.isDirectory()) {
						walk(file);
						continue;
					}

					// Skip version.json so esp32 can read it without decompressing
					if (path.basename(file) === 'version.json' && dir === outputDir) {
						continue;
					}

					const ext = path.extname(file).toLowerCase();

					if (!COMPRESSIBLE_EXTENSIONS.has(ext)) {
						continue;
					}

					const data = fs.readFileSync(file);

					fs.writeFileSync(
						file + '.gz',
						zlib.gzipSync(data, {
							level: zlib.constants.Z_BEST_COMPRESSION
						})
					);

					fs.unlinkSync(file);
				}
			}

			walk(outputDir);
		}
	};
}
