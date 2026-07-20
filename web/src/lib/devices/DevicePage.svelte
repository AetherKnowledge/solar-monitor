<script lang="ts">
	import { goto } from '$app/navigation';
	import { resolve } from '$app/paths';
	import { page } from '$app/state';
	import { showError } from '$lib/popup/Popup.svelte';
	import ActionBar from './Cards/ActionBar.svelte';
	import Header from './Cards/Header.svelte';
	import MqttCard from './Cards/ModbusDetails.svelte';
	import GeneralCard from './Cards/MqttDetails.svelte';
	import RegisterTable from './Cards/RegisterTable.svelte';
	import type { RegisterListItem } from './Cards/RegisterTableRow.svelte';
	import { createDevicesController, devicesState } from './DeviceController.svelte';
	import { RegisterType } from './DeviceTypes';

	const deviceId = $derived(page.params.id);

	const { query, save, cancel } = createDevicesController();
	let device = $derived(devicesState.devices?.find((d) => d.identifier === deviceId));

	$effect(() => {
		if (!device && devicesState.initialized) {
			showError('Device not found', () => {
				goto(resolve('/devices'));
			});
		}
	});

	let registers: RegisterListItem[] = $derived.by(() => {
		if (!device) return [];

		const readRegisters = device.readRegisters.map((r) => ({
			id: r.discovery.unique_id,
			type: RegisterType.Read,
			name: r.discovery.name,
			address: r.address,
			value: '0',
			unit: r.discovery.unit_of_measurement
		}));

		const virtualRegisters = device.virtualSensors.map((r) => ({
			id: r.discovery.unique_id,
			type: RegisterType.Virtual,
			name: r.discovery.name,
			value: '0',
			unit: r.discovery.unit_of_measurement
		}));

		const selectRegisters = device.selectWriteRegisters.map((r) => ({
			id: r.discovery.unique_id,
			type: RegisterType.Select,
			name: r.discovery.name,
			address: r.address,
			value: '0',
			unit: r.discovery.unit_of_measurement
		}));

		const numberRegisters = device.numberWriteRegisters.map((r) => ({
			id: r.discovery.unique_id,
			type: RegisterType.Number,
			name: r.discovery.name,
			address: r.address,
			value: '0',
			unit: r.discovery.unit_of_measurement
		}));

		return [...readRegisters, ...virtualRegisters, ...selectRegisters, ...numberRegisters];
	});

	$effect(() => {
		console.log(query);
	});
</script>

{#if device}
	<Header {device} />

	<div class="grid gap-6 xl:grid-cols-2 pb-6">
		<GeneralCard bind:device />
		<MqttCard bind:device />
	</div>
	<div class="grid gap-6 pb-6">
		<RegisterTable {registers} />
	</div>

	<ActionBar
		hasChanged={devicesState.hasChanged}
		isSaving={false}
		onSave={save}
		onCancel={cancel}
	/>
{/if}
