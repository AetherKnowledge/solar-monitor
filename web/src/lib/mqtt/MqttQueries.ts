import { apiFetch } from '$lib/common/CommonFunctions';
import type { SimpleResponse } from '$lib/common/CommonTypes';
import { queryOptions } from '@tanstack/svelte-query';
import type { MqttConfig } from './MqttTypes';

export function createMqttConfigQueryOptions() {
	return queryOptions<MqttConfig>({
		queryKey: ['mqttConfig'],
		queryFn: async () => apiFetch<MqttConfig>(`/api/config/mqtt`)
	});
}

export async function updateMqttConfig(config: MqttConfig) {
	const result = await apiFetch<SimpleResponse>(`/api/config/mqtt`, {
		method: 'POST',
		headers: {
			'Content-Type': 'application/json'
		},
		body: JSON.stringify(config)
	});

	console.log('MQTT config updated:', result.message);
}
