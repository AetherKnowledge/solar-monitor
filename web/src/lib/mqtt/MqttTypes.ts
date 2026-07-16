export type MqttConfig = {
	host: string;
	port: number;
	username?: string;
	password?: string;
	autoDiscoveryEnabled: boolean;
	autoDiscoveryPrefix?: string;
	clientId?: string;
};

export const defaultMqttConfig: MqttConfig = {
	host: 'http://localhost',
	port: 1883,
	autoDiscoveryEnabled: false
};
