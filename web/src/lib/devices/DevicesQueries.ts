import { apiFetch } from '$lib/common/CommonFunctions';
import type { NetworkConfig } from '$lib/network/NetworkTypes';

export function createDevicesConfigQueryOptions() {
	return {
		queryKey: ['devicesConfig'],
		queryFn: async () => apiFetch<NetworkConfig>(`/api/config/modbus`)
	};
}
