import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import test from 'node:test';
import { loadRunes } from './helpers/runes.mjs';

const { createDevicePatch } = await loadRunes(
	readFileSync(new URL('../src/lib/common/Patcher.svelte.ts', import.meta.url), 'utf8'),
	'Patcher.svelte.js'
);
const { state } = await loadRunes(
	'export function state(value) { let result = $state(value); return result; }',
	'fixture.svelte.js'
);

const collections = [
	'readRegisters',
	'virtualSensors',
	'selectWriteRegisters',
	'numberWriteRegisters'
];

function device() {
	return {
		discovery: { identifier: 'inverter', name: 'Inverter', manufacturer: 'Test' },
		mqttEnabled: true,
		slaveId: 1,
		timeout: 1000,
		baudrate: 9600,
		port: 1,
		swapBytes: false,
		...Object.fromEntries(collections.map((key) => [key, []]))
	};
}

function entity(id) {
	return {
		address: 10,
		rounding: 2,
		transform: 'Divide',
		transformArgument: 10,
		signedValue: true,
		expression: 'voltage * 2',
		isPersistent: true,
		discovery: { unique_id: id, name: id, options: ['Off', 'On'] }
	};
}

for (const key of collections) {
	test(`${key}: adding a reactive entity produces an independent plain patch`, () => {
		const original = device();
		const current = state(structuredClone(original));
		current[key].push(entity('new'));
		const patch = createDevicePatch(original, current);
		assert.deepEqual(patch, { [key]: [entity('new')] });
		assert.doesNotThrow(() => structuredClone(patch));
		current[key][0].discovery.name = 'Changed later';
		current[key][0].discovery.options.push('Auto');
		assert.equal(patch[key][0].discovery.name, 'new');
		assert.deepEqual(patch[key][0].discovery.options, ['Off', 'On']);
	});

	test(`${key}: deleting the sole entity produces an explicit deletion`, () => {
		const original = device();
		original[key].push(entity('removed'));
		const current = state(structuredClone(original));
		current[key] = [];
		assert.deepEqual(createDevicePatch(original, current), {
			[key]: [{ discovery: { unique_id: 'removed' }, deleted: true }]
		});
	});

	test(`${key}: mixed edits, additions and deletions preserve untouched entities`, () => {
		const original = device();
		original[key] = ['keep', 'edit', 'remove'].map(entity);
		const current = state(structuredClone(original));
		current[key][1].discovery.name = 'Edited';
		current[key].pop();
		current[key].push(entity('add'));
		assert.deepEqual(createDevicePatch(original, current), {
			[key]: [
				{ discovery: { unique_id: 'edit', name: 'Edited' } },
				entity('add'),
				{ discovery: { unique_id: 'remove' }, deleted: true }
			]
		});
	});
}

test('unchanged reactive data and live value updates do not produce patches', () => {
	const original = device();
	for (const key of collections) original[key].push({ ...entity(key), value: 1 });
	const current = state(structuredClone(original));
	assert.equal(createDevicePatch(original, current), undefined);
	for (const key of collections) current[key][0].value = 2;
	assert.equal(createDevicePatch(original, current), undefined);
});

test('adding then removing an unsaved entity does not produce a deletion', () => {
	const original = device();
	const current = state(structuredClone(original));
	current.readRegisters.push(entity('temporary'));
	current.readRegisters.pop();
	assert.equal(createDevicePatch(original, current), undefined);
});
