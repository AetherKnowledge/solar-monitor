import { apiFetch } from '$lib/common/CommonFunctions';
import { UpdateStatus, type SimpleResponse, type WithUpdateStatus } from '$lib/common/CommonTypes';
import { type ScanRequestResponse } from '$lib/network/NetworkTypes';
import { createQuery } from '@tanstack/svelte-query';
import type { NetworkConfig } from './NetworkTypes';

export function createNetworkScanQuery() {
	return createQuery<ScanRequestResponse>(() => ({
		queryKey: ['networks'],
		queryFn: async () => getNetworks(),
		initialData: {
			status: UpdateStatus.NotStarted,
			networks: []
		},
		refetchInterval: (query) => {
			const status = query.state.data?.status;
			if (status === UpdateStatus.InProgress || status === UpdateStatus.Requested) {
				return 1000; // Refetch every second while scanning
			}
		}
	}));
}

const getNetworks = async () => {
	const result = await apiFetch<ScanRequestResponse>(`/api/network/wifinetworks`);

	if (result.status === UpdateStatus.NotStarted) {
		const scanResult = await scanNetworks();
		result.status = scanResult;
	}

	return result;
};

export const scanNetworks = async () => {
	return await apiFetch<UpdateStatus>(`/api/network/wifinetworks/scan`, {
		method: 'POST'
	});
};

export function createNetworkConfigQuery() {
	return createQuery<WithUpdateStatus<NetworkConfig>>(() => ({
		queryKey: ['networkConfig'],
		queryFn: async () => apiFetch(`/api/network/config`),
		refetchInterval: (query) => {
			const status = query.state.data?.updateStatus;
			if (status === UpdateStatus.InProgress || status === UpdateStatus.Requested) {
				return 1000; // Refetch every second while scanning
			}
		}
	}));
}

export async function updateNetworkConfig(config: NetworkConfig) {
	const result = await apiFetch<SimpleResponse>(`/api/network/config`, {
		method: 'POST',
		headers: {
			'Content-Type': 'application/json'
		},
		body: JSON.stringify(config)
	});

	console.log('Network config updated:', result.message);
}
