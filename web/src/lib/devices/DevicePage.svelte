<script lang="ts">
	import { goto } from '$app/navigation';
	import { resolve } from '$app/paths';
	import { page } from '$app/state';
	import { showError } from '$lib/popup/Popup.svelte';
	import ActionBar from './Cards/ActionBar.svelte';
	import AddRegister from './Cards/AddRegister.svelte';
	import Header from './Cards/Header.svelte';
	import MqttCard from './Cards/ModbusDetails.svelte';
	import GeneralCard from './Cards/MqttDetails.svelte';
	import PopupBase from './Cards/PopupBase.svelte';
	import RegisterTable from './Cards/RegisterTable.svelte';
	import { createDevicesController, devicesState } from './DeviceController.svelte';
	import {
		addRegister,
		getDefaultRegister,
		RegisterType,
		removeRegister,
		type RegisterItem,
		type RegisterListItem
	} from './DeviceTypes';

	const deviceId = $derived(page.params.id);
	let newRegister: RegisterItem | null = $state(null);

	const { query, save, cancel } = createDevicesController();
	let device = $derived(devicesState.devices?.find((d) => d.discovery.identifier === deviceId));
	let showAddRegisterPopup = $state(false);

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
			value: r.value ?? 0,
			unit: r.discovery.unit_of_measurement,

			data: {
				register: r,
				type: RegisterType.Read
			}
		})) satisfies RegisterListItem[];

		const virtualRegisters = device.virtualSensors.map((r) => ({
			id: r.discovery.unique_id,
			type: RegisterType.Virtual,
			name: r.discovery.name,
			value: r.value ?? 0,
			unit: r.discovery.unit_of_measurement,
			data: {
				type: RegisterType.Virtual,
				register: r
			}
		})) satisfies RegisterListItem[];

		const selectRegisters = device.selectWriteRegisters.map((r) => ({
			id: r.discovery.unique_id,
			type: RegisterType.Select,
			name: r.discovery.name,
			address: r.address,
			value: r.value ?? 0,
			unit: r.discovery.unit_of_measurement,
			data: {
				type: RegisterType.Select,
				register: r
			}
		})) satisfies RegisterListItem[];

		const numberRegisters = device.numberWriteRegisters.map((r) => ({
			id: r.discovery.unique_id,
			type: RegisterType.Number,
			name: r.discovery.name,
			address: r.address,
			value: r.value ?? 0,
			unit: r.discovery.unit_of_measurement,
			data: {
				type: RegisterType.Number,
				register: r
			}
		})) satisfies RegisterListItem[];

		return [...readRegisters, ...virtualRegisters, ...selectRegisters, ...numberRegisters];
	});

	let selectedRegister: RegisterListItem | null = $state(null);

	$effect(() => {
		console.log('hasChanged:', devicesState.hasChanged);
		console.log('Query Data:', JSON.stringify(query.data?.devices));
		console.log('Device Data:', JSON.stringify(devicesState.devices));
	});
</script>

{#if device}
	<Header {device} />

	<div class="grid gap-6 pb-6 xl:grid-cols-2">
		<GeneralCard bind:device />
		<MqttCard bind:device />
	</div>
	<div class="grid gap-6 pb-6">
		<RegisterTable
			{registers}
			onEdit={(register) => (selectedRegister = register)}
			onAddRegister={() => (showAddRegisterPopup = true)}
		/>
	</div>

	<ActionBar
		hasChanged={devicesState.hasChanged}
		isSaving={false}
		onSave={save}
		onCancel={cancel}
	/>

	{#if selectedRegister}
		<PopupBase
			bind:register={selectedRegister.data}
			onSave={() => (selectedRegister = null)}
			onCancel={() => {
				selectedRegister = null;
				cancel();
			}}
			onDelete={() => {
				if (device && selectedRegister) {
					removeRegister(device, selectedRegister.data);
				}
				selectedRegister = null;
			}}
		/>
	{/if}

	{#if newRegister}
		<PopupBase
			bind:register={newRegister}
			isNew={true}
			onSave={() => {
				if (device && newRegister) {
					addRegister(device, newRegister);
				}
				newRegister = null;
			}}
			onCancel={() => {
				newRegister = null;
			}}
		/>
	{/if}

	{#if showAddRegisterPopup}
		<AddRegister
			onCancel={() => (showAddRegisterPopup = false)}
			onSelect={(type) => {
				showAddRegisterPopup = false;
				newRegister = getDefaultRegister(type);
			}}
		/>
	{/if}
{/if}
