import { createRequire, stripTypeScriptTypes } from 'node:module';
import { pathToFileURL } from 'node:url';
import { compileModule } from 'svelte/compiler';

const require = createRequire(import.meta.url);

export async function loadRunes(source, filename) {
	const compiled = compileModule(stripTypeScriptTypes(source), {
		filename,
		generate: 'client'
	}).js.code;
	const code = compiled.replace(/(['"])svelte\/internal\/([^'"]+)\1/g, (_, quote, name) =>
		JSON.stringify(pathToFileURL(require.resolve('svelte/internal/' + name)).href)
	);
	return import(
		'data:text/javascript;base64,' +
			Buffer.from(code + '\n//# sourceURL=' + filename).toString('base64')
	);
}
