import { apiFetch } from '$lib/common/CommonFunctions';
import { UpdateStatus, type SimpleResponse } from '$lib/common/CommonTypes';
import { hidePopup, showError, showLoading, showSuccess } from '$lib/popup/Popup.svelte';
import { createQuery } from '@tanstack/svelte-query';
import { onMount } from 'svelte';
import { createDevicePatch, type Patch } from '../common/Patcher.svelte';
import { RegisterType, type ModbusDevice, type ModbusDeviceInfo } from './DeviceTypes';

export type ModbusDevices = {
	devices: ModbusDeviceInfo[];
};

export type ModbusDeviceQuery = {
	device: ModbusDevice;
};

type RegisterValues = Record<string, number>;

type ModbusValues = {
	readRegisters?: RegisterValues;
	virtualSensors?: RegisterValues;
	selectWriteRegisters?: RegisterValues;
	numberWriteRegisters?: RegisterValues;
};

export function createDevicesController() {
	const query = createQuery<ModbusDevices>(() => ({
		queryKey: ['devicesInfo'],
		queryFn: async () => apiFetch(`/api/modbus/devices`)
	}));

	$effect(() => {
		if (query.isPending) {
			showLoading('Loading device configuration...');
		} else {
			hidePopup();
		}
	});

	return {
		query
	};
}

export function createDeviceController(deviceId: string) {
	const query = createQuery<ModbusDeviceQuery>(() => ({
		queryKey: ['device-' + deviceId],
		queryFn: async () => apiFetch(`/api/modbus/devices?id=${deviceId}`),
		refetchInterval: 30000
	}));

	const statusQuery = createQuery<{ updateStatus: UpdateStatus }>(() => ({
		queryKey: ['updateStatus'],
		queryFn: async () => apiFetch(`/api/modbus/devices/status?id=${deviceId}`),
		initialData: { updateStatus: UpdateStatus.NotStarted },
		initialDataUpdatedAt: Date.now(),
		staleTime: 0,
		refetchInterval: (query) => {
			const status = query.state.data?.updateStatus;
			if (status === UpdateStatus.InProgress || status === UpdateStatus.Requested) {
				return 1000;
			}
		}
	}));

	const valuesQuery = createQuery<ModbusValues>(() => ({
		queryKey: ['device-' + deviceId + '-values'],
		queryFn: async () => apiFetch(`/api/modbus/values?id=${deviceId}`),
		refetchInterval: 5000,
		initialDataUpdatedAt: Date.now(),
		staleTime: 0
	}));

	const savedDevice = $derived(query.data);
	let hasUpdated: boolean = $state(false);

	const deviceState = $state({
		device: undefined as ModbusDevice | undefined,
		initialized: false
	});

	$effect(() => {
		if (!deviceState.initialized && !query.isPending) {
			deviceState.device = savedDevice?.device;
			deviceState.initialized = true;
		}
	});

	$effect(() => {
		if (query.isPending) {
			showLoading('Loading device configuration...');
		} else {
			hidePopup();
		}
	});

	$effect(() => {
		if (
			statusQuery.data?.updateStatus === UpdateStatus.InProgress ||
			statusQuery.data?.updateStatus === UpdateStatus.Requested
		) {
			hasUpdated = true;
			showLoading('Updating device configuration...');
		} else if (statusQuery.data?.updateStatus === UpdateStatus.UpdateComplete && hasUpdated) {
			showSuccess('Update completed successfully.');
			query.refetch();
			hasUpdated = false;
		} else if (statusQuery.data?.updateStatus === UpdateStatus.UpdateFailed && hasUpdated) {
			showError('Update failed. Please try again.');
			cancel();
			hasUpdated = false;
		}
	});

	$effect(() => {
		deviceState.device?.readRegisters.forEach((register) => {
			register.value = getDeviceValues(
				valuesQuery.data || {},
				RegisterType.Read,
				register.discovery.unique_id
			);
		});
		deviceState.device?.virtualSensors.forEach((sensor) => {
			sensor.value = getDeviceValues(
				valuesQuery.data || {},
				RegisterType.Virtual,
				sensor.discovery.unique_id
			);
		});
		deviceState.device?.selectWriteRegisters.forEach((register) => {
			register.value = getDeviceValues(
				valuesQuery.data || {},
				RegisterType.Select,
				register.discovery.unique_id
			);
		});
		deviceState.device?.numberWriteRegisters.forEach((register) => {
			register.value = getDeviceValues(
				valuesQuery.data || {},
				RegisterType.Number,
				register.discovery.unique_id
			);
		});
	});

	onMount(() => {
		return () => {
			hidePopup();
		};
	});

	async function save() {
		if (!savedDevice || !deviceState.device || !hasChanged) {
			return;
		}

		const patch = createDevicePatch(savedDevice.device, deviceState.device);

		if (!patch) {
			return;
		}

		await updateDeviceConfig(savedDevice.device.discovery.identifier, patch);

		showLoading('Updating device configuration...');
		await statusQuery.refetch();
	}

	async function cancel() {
		if (savedDevice) {
			deviceState.device = { ...savedDevice.device };
		}
	}

	const hasChanged = $derived.by(() => {
		if (!savedDevice || !deviceState.device) {
			return false;
		}

		return createDevicePatch(savedDevice.device, deviceState.device) !== undefined;
	});

	return {
		query,
		get state() {
			return deviceState;
		},
		set state(value) {
			deviceState.device = value.device;
			deviceState.initialized = value.initialized;
		},
		save,
		cancel,
		get hasChanged() {
			return hasChanged;
		}
	};
}

export async function updateDeviceConfig(
	identifier: string,
	patch: Patch<ModbusDevice>
): Promise<boolean> {
	return await apiFetch<SimpleResponse>(`/api/modbus/devices?id=${identifier}`, {
		method: 'POST',
		headers: {
			'Content-Type': 'application/json'
		},
		body: JSON.stringify(patch)
	})
		.then(() => true)
		.catch(() => false);
}

function getDeviceValues(
	values: ModbusValues,
	registerType: RegisterType,
	uniqueId: string
): number {
	switch (registerType) {
		case RegisterType.Read:
			return values.readRegisters?.[uniqueId] ?? 0;
		case RegisterType.Virtual:
			return values.virtualSensors?.[uniqueId] ?? 0;
		case RegisterType.Select:
			return values.selectWriteRegisters?.[uniqueId] ?? 0;
		case RegisterType.Number:
			return values.numberWriteRegisters?.[uniqueId] ?? 0;
	}
}
