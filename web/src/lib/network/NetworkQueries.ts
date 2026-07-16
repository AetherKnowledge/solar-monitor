import { UpdateStatus } from '$lib/common/CommonTypes';
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
	const response = await fetch(`/api/network/wifinetworks`);
	const result: ScanRequestResponse = await response.json();

	if (result.status === UpdateStatus.NotStarted) {
		const scanResult = await scanNetworks();
		result.status = scanResult.status;
	}

	return result;
};

export const scanNetworks = async () => {
	const response = await fetch(`/api/network/wifinetworks/scan`, {
		method: 'POST'
	});
	return (await response.json()) as { status: UpdateStatus };
};

export function createNetworkConfigQueryOptions() {
	return queryOptions<NetworkConfig>({
		queryKey: ['networkConfig'],
		queryFn: async () => getNetworkConfig()
	});
}

const getNetworkConfig = async () => {
	const response = await fetch(`/api/config/network`);
	const result: NetworkConfig = await response.json();

	return result;
};

export async function updateNetworkConfig(config: NetworkConfig) {
	const result = await fetch(`/api/config/network`, {
		method: 'POST',
		headers: {
			'Content-Type': 'application/json'
		},
		body: JSON.stringify(config)
	});

	console.log('Network config updated:', await result.text());
}
