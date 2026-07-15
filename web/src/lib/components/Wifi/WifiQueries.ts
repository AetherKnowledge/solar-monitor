import { type ScanRequestResponse, WifiScanStatus } from '$lib/types/NetworkConfig';
import { queryOptions } from '@tanstack/svelte-query';

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

	if (result.status === 'not_started') {
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
