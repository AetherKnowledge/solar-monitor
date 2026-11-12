import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import test from 'node:test';
import { compile } from 'svelte/compiler';
import { effect_root, flush as flush_sync } from 'svelte/internal/client';
import { loadRunes } from './helpers/runes.mjs';

const controllerSource = readFileSync(
	new URL('../src/lib/update/UpdateController.svelte.ts', import.meta.url),
	'utf8'
);
const pageSource = readFileSync(
	new URL('../src/lib/update/UpdatePage.svelte', import.meta.url),
	'utf8'
);
const UpdateStatus = {
	NotStarted: 'not_started',
	Requested: 'requested',
	InProgress: 'in_progress',
	UpdateComplete: 'complete',
	UpdateFailed: 'failed'
};
const { state } = await loadRunes(
	'export function state(value) { let result = $state(value); return result; }',
	'update-state.svelte.js'
);
let sequence = 0;

async function loadSubject(source, bindings, exports = '') {
	const key = `__update_test_${sequence++}`;
	globalThis[key] = bindings;
	try {
		return await loadRunes(
			`const { ${Object.keys(bindings).join(', ')} } = globalThis[${JSON.stringify(key)}];\n` +
				source.replace(/^\s*import\s[\s\S]*?;\r?$/gm, '') +
				exports,
			`${key}.svelte.js`
		);
	} finally {
		delete globalThis[key];
	}
}

function popupBindings() {
	const popup = { type: 'none', message: '' };
	return {
		popup,
		hidePopup: () => {
			popup.type = 'none';
		},
		showLoading: (message) => {
			popup.type = 'loading';
			popup.message = message;
		},
		showError: (message) => {
			popup.type = 'error';
			popup.message = message;
		}
	};
}

function asset(board, name) {
	return {
		version: 'v2.0.0',
		file: `${name}.bin`,
		url: `https://example.com/${board}_${name}.bin`,
		size: 10,
		sha256: 'a'.repeat(64),
		signature: 'signed-asset'
	};
}

async function loadController(bindings) {
	return loadSubject(controllerSource, {
		UpdateStatus,
		__WEBSITE_VERSION__: '1.1.2',
		...popupBindings(),
		...bindings
	});
}

async function loadPage(overrides = {}) {
	const popups = popupBindings();
	const mounts = [];
	const statusQuery = { refetch: async () => {} };
	const page = await loadSubject(
		pageSource.match(/<script lang="ts">([\s\S]*?)<\/script>/)[1],
		{
			...popups,
			onMount: (callback) => mounts.push(callback),
			createUpdateController: () => ({ versionQuery: { data: {} }, statusQuery }),
			checkForUpdates: async () => {},
			updateFirmware: async () => {},
			updateWebsite: async () => {},
			showYesNo: async () => true,
			...overrides
		},
		'\nexport { onCheckUpdate, onUpdateFirmware, onUpdateWebsite };\n' +
			'export function isChecking() { return checkingForUpdates; }'
	);
	return { page, ...popups, mounts, statusQuery };
}

test('both boards select and submit their own signed firmware and website assets', async () => {
	const boards = ['esp32dev', 'esp32-s3-devkitc1-n16r8'];
	const manifest = {
		boards: Object.fromEntries(
			boards.map((board) => [
				board,
				{
					firmware: asset(board, 'firmware'),
					website: asset(board, 'website')
				}
			])
		)
	};
	let boardType;
	const posts = [];
	const controller = await loadController({
		apiFetch: async (url, init) => {
			if (url === '/api/system/board') return { boardType };
			if (!init?.method) return manifest;
			posts.push({ url, body: JSON.parse(init.body) });
		}
	});
	for (boardType of boards) {
		assert.deepEqual(await controller.checkForUpdates(), manifest.boards[boardType]);
		await controller.updateFirmware();
		await controller.updateWebsite();
		assert.deepEqual(posts.at(-2), {
			url: '/api/update/firmware',
			body: manifest.boards[boardType].firmware
		});
		assert.deepEqual(posts.at(-1), {
			url: '/api/update/website',
			body: manifest.boards[boardType].website
		});
	}
});

test('absent, old, or incomplete board manifests reject clearly before installation', async () => {
	let manifest;
	const controller = await loadController({
		apiFetch: async (url, init) => {
			assert.equal(init?.method, undefined, 'must not submit an invalid release');
			return url === '/api/system/board' ? { boardType: 'esp32dev' } : manifest;
		}
	});
	for (manifest of [
		null,
		{},
		{ boards: {} },
		{ boards: { esp32dev: {} } },
		{ boards: { esp32dev: { firmware: asset('esp32dev', 'firmware') } } }
	]) {
		for (const action of [
			controller.checkForUpdates,
			controller.updateFirmware,
			controller.updateWebsite
		]) {
			await assert.rejects(action, /No update is available for board "esp32dev"/);
		}
	}
});

test('missing board identification reports an actionable error', async () => {
	const controller = await loadController({ apiFetch: async () => ({}) });
	await assert.rejects(controller.getBoardManifest, /Device did not report its board type/);
});

test('failed checks reset busy state, prevent duplicate requests, and allow retry', async () => {
	let rejectCheck;
	let attempts = 0;
	const { page, popup } = await loadPage({
		checkForUpdates: () => {
			attempts++;
			return attempts === 1
				? new Promise((_, reject) => {
						rejectCheck = reject;
					})
				: Promise.resolve();
		}
	});
	const checking = page.onCheckUpdate();
	assert.equal(page.isChecking(), true);
	await page.onCheckUpdate();
	assert.equal(attempts, 1);
	rejectCheck(new Error('No update is available for this board.'));
	await checking;
	assert.equal(page.isChecking(), false);
	assert.equal(popup.type, 'error');
	assert.match(popup.message, /No update is available/);
	await page.onCheckUpdate();
	assert.equal(attempts, 2);
	assert.equal(page.isChecking(), false);
});

test('automatic checks use the same error recovery as button clicks', async () => {
	const { page, popup, mounts } = await loadPage({
		checkForUpdates: async () => {
			throw new Error('Network unavailable');
		}
	});
	const cleanup = mounts[0]();
	await new Promise((resolve) => setImmediate(resolve));
	assert.equal(page.isChecking(), false);
	assert.equal(popup.type, 'error');
	assert.equal(popup.message, 'Network unavailable');
	cleanup();
	assert.equal(popup.type, 'none');
});

for (const kind of ['Firmware', 'Website']) {
	test(`${kind} install failures replace loading popup and allow retry`, async () => {
		let attempts = 0;
		const { page, popup, statusQuery } = await loadPage({
			[`update${kind}`]: async () => {
				if (++attempts === 1) throw new Error('Device rejected update');
			}
		});
		let refetches = 0;
		statusQuery.refetch = async () => {
			refetches++;
		};
		await page[`onUpdate${kind}`]();
		assert.equal(popup.type, 'error');
		assert.equal(popup.message, 'Device rejected update');
		assert.equal(refetches, 0);
		await page[`onUpdate${kind}`]();
		assert.equal(attempts, 2);
		assert.equal(refetches, 1);
	});

	test(`canceling ${kind} installation sends no request`, async () => {
		const { page, popup } = await loadPage({
			showYesNo: async () => false,
			[`update${kind}`]: () => assert.fail('canceled update must not start')
		});
		await page[`onUpdate${kind}`]();
		assert.equal(popup.type, 'none');
	});
}

test('polling reports device failures once and preserves errors and manual upload popups', async () => {
	const popups = popupBindings();
	const statusQuery = state({ data: { status: UpdateStatus.NotStarted, hasTotalSize: false } });
	let queryCount = 0;
	const controller = await loadController({
		...popups,
		createQuery: () => (++queryCount === 1 ? { data: {} } : statusQuery)
	});
	const stop = effect_root(() => {
		controller.createUpdateController();
	});
	try {
		popups.showError('Request failed');
		flush_sync();
		assert.equal(popups.popup.message, 'Request failed');
		assert.equal(popups.popup.type, 'error');
		flush_sync(() => {
			statusQuery.data.status = UpdateStatus.Requested;
		});
		assert.equal(popups.popup.type, 'loading');
		flush_sync(() => {
			statusQuery.data.status = UpdateStatus.UpdateFailed;
		});
		assert.equal(popups.popup.type, 'error');
		assert.match(popups.popup.message, /Update failed/);
		popups.hidePopup();
		flush_sync(() => {
			statusQuery.data = { ...statusQuery.data };
		});
		assert.equal(popups.popup.type, 'none', 'same failure must not reopen dismissed popup');
		popups.showError('Another request failed');
		flush_sync(() => {
			statusQuery.data.status = UpdateStatus.NotStarted;
		});
		assert.equal(popups.popup.type, 'error', 'idle polling must not dismiss errors');
		flush_sync(() => {
			statusQuery.data = { status: UpdateStatus.InProgress, hasTotalSize: true };
		});
		popups.showLoading('Uploading firmware...', 42);
		flush_sync(() => {
			statusQuery.data.status = UpdateStatus.UpdateComplete;
		});
		assert.equal(popups.popup.message, 'Uploading firmware...');
	} finally {
		stop();
	}
});

test('update page compiles with error handlers', () => {
	assert.doesNotThrow(() =>
		compile(pageSource, { filename: 'UpdatePage.svelte', generate: 'client' })
	);
});
