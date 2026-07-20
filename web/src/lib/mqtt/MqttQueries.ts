import { apiFetch } from '$lib/common/CommonFunctions';
import type { SimpleResponse } from '$lib/common/CommonTypes';
import { createQuery } from '@tanstack/svelte-query';
import type { MqttConfig } from './MqttTypes';

export function createMqttConfigQuery() {
	return createQuery<MqttConfig>(() => ({
		queryKey: ['mqttConfig'],
		queryFn: async () => apiFetch<MqttConfig>(`/api/mqtt/config`)
	}));
}

export async function updateMqttConfig(config: MqttConfig) {
	const result = await apiFetch<SimpleResponse>(`/api/mqtt/config`, {
		method: 'POST',
		headers: {
			'Content-Type': 'application/json'
		},
		body: JSON.stringify(config)
	});

	console.log('MQTT config updated:', result.message);
}
