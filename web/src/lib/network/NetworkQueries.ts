import { apiFetch } from '$lib/common/CommonFunctions';
import { UpdateStatus, type SimpleResponse } from '$lib/common/CommonTypes';
import { type ScanRequestResponse } from '$lib/network/NetworkTypes';
import { queryOptions } from '@tanstack/svelte-query';
import type { NetworkConfig } from './NetworkTypes';

export function createNetworkQueryOptions() {
	return queryOptions<ScanRequestResponse>({
		queryKey: ['networks'],
		queryFn: async () => getNetworks(),
		initialData: {
			status: UpdateStatus.NotStarted,
			networks: []
		},
		refetchInterval: (query) => {
			const status = query.state.data?.status;
			if (status === UpdateStatus.InProgress) {
				return 1000; // Refetch every second while scanning
			}
		}
	});
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

export function createNetworkConfigQueryOptions() {
	return queryOptions<NetworkConfig>({
		queryKey: ['networkConfig'],
		queryFn: async () => apiFetch<NetworkConfig>(`/api/config/network`)
	});
}

export async function updateNetworkConfig(config: NetworkConfig) {
	const result = await apiFetch<SimpleResponse>(`/api/config/network`, {
		method: 'POST',
		headers: {
			'Content-Type': 'application/json'
		},
		body: JSON.stringify(config)
	});

	console.log('Network config updated:', result.message);
}
