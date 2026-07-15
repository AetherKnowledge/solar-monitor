import { Status } from '$lib/common/CommonTypes';
import { type ScanRequestResponse } from '$lib/network/NetworkTypes';
import { queryOptions } from '@tanstack/svelte-query';
import type { NetworkConfig } from './NetworkTypes';

export function createNetworkQueryOptions() {
	return queryOptions<ScanRequestResponse>({
		queryKey: ['networks'],
		queryFn: async () => getNetworks(),
		initialData: {
			status: Status.NotStarted,
			networks: []
		},
		refetchInterval: (query) => {
			const status = query.state.data?.status;
			if (status === Status.InProgress) {
				return 1000; // Refetch every second while scanning
			}
		}
	});
}

const getNetworks = async () => {
	const response = await fetch(`/api/network/wifinetworks`);
	const result: ScanRequestResponse = await response.json();

	if (result.status === Status.NotStarted) {
		const scanResult = await scanNetworks();
		result.status = scanResult.status;
	}

	return result;
};

export const scanNetworks = async () => {
	const response = await fetch(`/api/network/wifinetworks/scan`, {
		method: 'POST'
	});
	return (await response.json()) as { status: Status };
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
	await fetch(`/api/config/network`, {
		method: 'POST',
		headers: {
			'Content-Type': 'application/json'
		},
		body: JSON.stringify(config)
	});
}
