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
	import { createDeviceController } from './DeviceController.svelte';
	import {
		addRegister,
		getDefaultRegister,
		RegisterType,
		removeRegister,
		type RegisterItem,
		type RegisterListItem
	} from './DeviceTypes';

	let newRegister: RegisterItem | null = $state(null);
	const controller = createDeviceController(page.params.id || '');
	let showAddRegisterPopup = $state(false);

	$effect(() => {
		if (!controller.state.device && controller.state.initialized) {
			showError('Device not found', () => {
				goto(resolve('/devices'));
			});
		}
	});

	let registers: RegisterListItem[] = $derived.by(() => {
		if (!controller.state.device) return [];

		const readRegisters = controller.state.device.readRegisters.map((r) => ({
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

		const virtualRegisters = controller.state.device.virtualSensors.map((r) => ({
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

		const selectRegisters = controller.state.device.selectWriteRegisters.map((r) => ({
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

		const numberRegisters = controller.state.device.numberWriteRegisters.map((r) => ({
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
</script>

{#if controller.state.device}
	<Header device={controller.state.device} />

	<div class="grid gap-6 pb-6 xl:grid-cols-2">
		<GeneralCard bind:device={controller.state.device} />
		<MqttCard bind:device={controller.state.device} />
	</div>
	<div class="grid gap-6 pb-6">
		<RegisterTable
			{registers}
			onEdit={(register) => (selectedRegister = register)}
			onAddRegister={() => (showAddRegisterPopup = true)}
		/>
	</div>

	<ActionBar
		hasChanged={controller.hasChanged}
		isSaving={false}
		onSave={controller.save}
		onCancel={controller.cancel}
	/>

	{#if selectedRegister}
		<PopupBase
			bind:register={selectedRegister.data}
			onSave={() => (selectedRegister = null)}
			onCancel={() => {
				selectedRegister = null;
				controller.cancel();
			}}
			onDelete={() => {
				if (controller.state.device && selectedRegister) {
					removeRegister(controller.state.device, selectedRegister.data);
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
				if (controller.state.device && newRegister) {
					addRegister(controller.state.device, newRegister);
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
