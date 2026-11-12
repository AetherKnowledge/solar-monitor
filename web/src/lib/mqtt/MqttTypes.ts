export type MqttConfig = {
	host: string;
	port: number;
	username?: string;
	password?: string;
	hasPassword: boolean;
	autoDiscoveryEnabled: boolean;
	autoDiscoveryPrefix?: string;
	clientId?: string;
};

export const defaultMqttConfig: MqttConfig = {
	host: 'http://localhost',
	port: 1883,
	hasPassword: false,
	autoDiscoveryEnabled: false
};
