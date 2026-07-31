import { apiFetch } from '$lib/common/CommonFunctions';
import type { SimpleResponse } from '$lib/common/CommonTypes';
import type { Patch } from '$lib/common/Patcher.svelte';
import { createQuery } from '@tanstack/svelte-query';
import type { MqttConfig } from './MqttTypes';

export function createMqttConfigQuery() {
	return createQuery<MqttConfig>(() => ({
		queryKey: ['mqttConfig'],
		queryFn: async () => apiFetch<MqttConfig>(`/api/mqtt/config`)
	}));
}

export async function updateMqttConfig(patch: Patch<MqttConfig>): Promise<boolean> {
	return apiFetch<SimpleResponse>('/api/mqtt/config', {
		method: 'POST',
		headers: {
			'Content-Type': 'application/json'
		},
		body: JSON.stringify(patch)
	})
		.then(() => true)
		.catch(() => false);
}
