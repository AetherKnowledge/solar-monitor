import { apiFetch } from '$lib/common/CommonFunctions';
import type { ModbusDevice } from './DeviceTypes';

export function createDevicesConfigQueryOptions() {
	return {
		queryKey: ['devicesConfig'],
		queryFn: async () => apiFetch<ModbusDevice[]>(`/api/config/modbus`)
	};
}
