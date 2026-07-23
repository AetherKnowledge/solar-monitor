import { apiFetch } from '$lib/common/CommonFunctions';
import { UpdateStatus, type SimpleResponse } from '$lib/common/CommonTypes';
import { hidePopup, showError, showLoading } from '$lib/popup/Popup.svelte';
import { createQuery } from '@tanstack/svelte-query';
import { onMount } from 'svelte';
import { RegisterType, type ModbusDevice } from './DeviceTypes';

export type ModbusDevices = {
	devices: ModbusDevice[];
	updateStatus: UpdateStatus;
};

type RegisterValues = Record<string, number>;

type ModbusValues = Record<
	string,
	{
		readRegisters?: RegisterValues;
		virtualSensors?: RegisterValues;
		selectWriteRegisters?: RegisterValues;
		numberWriteRegisters?: RegisterValues;
	}
>;

export const devicesState = $state({
	devices: [] as ModbusDevice[],
	initialized: false,
	hasChanged: false
});

export function createDevicesController() {
	const query = createQuery<ModbusDevices>(() => ({
		queryKey: ['devicesConfig'],
		queryFn: async () => apiFetch(`/api/modbus/config`),
		refetchInterval: (query) => {
			const status = query.state.data?.updateStatus;
			if (status === UpdateStatus.InProgress || status === UpdateStatus.Requested) {
				return 1000; // Refetch every second while scanning
			}
		}
	}));

	const valuesQuery = createQuery<ModbusValues>(() => ({
		queryKey: ['devicesValues'],
		queryFn: async () => apiFetch(`/api/modbus/values`),
		refetchInterval: 5000,
		initialDataUpdatedAt: Date.now(),
		staleTime: 0
	}));

	const savedDevices = $derived(query.data?.devices || []);

	$effect(() => {
		devicesState.hasChanged = JSON.stringify(devicesState.devices) !== JSON.stringify(savedDevices);
	});

	$effect(() => {
		if (!devicesState.initialized && !query.isPending) {
			devicesState.devices = [...savedDevices];
			devicesState.initialized = true;
		}
	});

	$effect(() => {
		if (
			query.isPending ||
			query.data?.updateStatus === UpdateStatus.InProgress ||
			query.data?.updateStatus === UpdateStatus.Requested
		) {
			showLoading('Loading device configuration...');
		} else {
			hidePopup();
		}
	});

	$effect(() => {
		devicesState.devices.forEach((device) => {
			device.readRegisters.forEach((register) => {
				register.value = getDeviceValues(
					valuesQuery.data || {},
					device.discovery.identifier,
					RegisterType.Read,
					register.discovery.unique_id
				);
			});
			device.virtualSensors.forEach((sensor) => {
				sensor.value = getDeviceValues(
					valuesQuery.data || {},
					device.discovery.identifier,
					RegisterType.Virtual,
					sensor.discovery.unique_id
				);
			});
			device.selectWriteRegisters.forEach((register) => {
				register.value = getDeviceValues(
					valuesQuery.data || {},
					device.discovery.identifier,
					RegisterType.Select,
					register.discovery.unique_id
				);
			});
			device.numberWriteRegisters.forEach((register) => {
				register.value = getDeviceValues(
					valuesQuery.data || {},
					device.discovery.identifier,
					RegisterType.Number,
					register.discovery.unique_id
				);
			});
		});
	});

	onMount(() => {
		return () => {
			hidePopup();
		};
	});

	async function save() {
		try {
			await updateDevicesConfig(devicesState.devices);
			await query.refetch();
		} catch {
			showError('Failed to save device configuration. Please try again.');
			return;
		}
	}

	async function cancel() {
		if (savedDevices.length > 0) {
			devicesState.devices = [...savedDevices];
		}
	}

	return {
		query,
		save,
		cancel
	};
}

export async function updateDevicesConfig(devices: ModbusDevice[]): Promise<boolean> {
	return await apiFetch<SimpleResponse>(`/api/modbus/config`, {
		method: 'POST',
		headers: {
			'Content-Type': 'application/json'
		},
		body: JSON.stringify({ devices })
	})
		.then(() => true)
		.catch(() => false);
}

function getDeviceValues(
	values: ModbusValues,
	deviceId: string,
	registerType: RegisterType,
	uniqueId: string
): number {
	const deviceValues = values[deviceId];
	if (!deviceValues) {
		return 0;
	}

	switch (registerType) {
		case RegisterType.Read:
			return deviceValues.readRegisters?.[uniqueId] ?? 0;
		case RegisterType.Virtual:
			return deviceValues.virtualSensors?.[uniqueId] ?? 0;
		case RegisterType.Select:
			return deviceValues.selectWriteRegisters?.[uniqueId] ?? 0;
		case RegisterType.Number:
			return deviceValues.numberWriteRegisters?.[uniqueId] ?? 0;
	}
	return 0;
}
