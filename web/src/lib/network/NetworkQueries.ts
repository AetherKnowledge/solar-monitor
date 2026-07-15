import { type ScanRequestResponse, WifiScanStatus } from '$lib/network/NetworkTypes';
import { queryOptions } from '@tanstack/svelte-query';
import type { NetworkConfig } from './NetworkTypes';

export function createNetworkQueryOptions() {
	return queryOptions<ScanRequestResponse>({
		queryKey: ['networks'],
		queryFn: async () => getNetworks(),
		initialData: {
			status: WifiScanStatus.SCAN_STATUS_NOT_STARTED,
			networks: []
		},
		refetchInterval: (query) => {
			const status = query.state.data?.status;
			if (status === WifiScanStatus.SCAN_STATUS_IN_PROGRESS) {
				return 1000; // Refetch every second while scanning
			}
		}
	});
}

const getNetworks = async () => {
	const response = await fetch(`/api/network/wifinetworks`);
	const result: ScanRequestResponse = await response.json();

	if (result.status === WifiScanStatus.SCAN_STATUS_NOT_STARTED) {
		const scanResult = await scanNetworks();
		result.status = scanResult.status;
	}

	return result;
};

export const scanNetworks = async () => {
	const response = await fetch(`/api/network/wifinetworks/scan`, {
		method: 'POST'
	});
	return (await response.json()) as { status: WifiScanStatus };
};

export function createNetworkConfigQueryOptions() {
	return queryOptions<NetworkConfig>({
		queryKey: ['networkConfig'],
		queryFn: async () => getNetworkConfig()
	});
}

const getNetworkConfig = async () => {
	const response = await fetch(`/api/network/config`);
	const result: NetworkConfig = await response.json();

	return result;
};
