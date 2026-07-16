import { queryOptions } from '@tanstack/svelte-query';
import type { MqttConfig } from './MqttTypes';

export function createMqttConfigQueryOptions() {
	return queryOptions<MqttConfig>({
		queryKey: ['mqttConfig'],
		queryFn: async () => getMqttConfig()
	});
}

const getMqttConfig = async () => {
	const response = await fetch(`/api/config/mqtt`);
	const result: MqttConfig = await response.json();

	return result;
};

export async function updateMqttConfig(config: MqttConfig) {
	const result = await fetch(`/api/config/mqtt`, {
		method: 'POST',
		headers: {
			'Content-Type': 'application/json'
		},
		body: JSON.stringify(config)
	});

	console.log('MQTT config updated:', await result.text());
}
