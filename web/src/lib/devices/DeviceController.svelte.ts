import { apiFetch } from '$lib/common/CommonFunctions';
import { UpdateStatus, type SimpleResponse } from '$lib/common/CommonTypes';
import { hidePopup, showError, showLoading } from '$lib/popup/Popup.svelte';
import { createQuery } from '@tanstack/svelte-query';
import type { ModbusDevice } from './DeviceTypes';

export type ModbusDevices = {
	devices: ModbusDevice[];
	updateStatus: UpdateStatus;
};

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
