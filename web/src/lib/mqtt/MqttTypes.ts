export type MqttConfig = {
	host: string;
	port: number;
	username?: string;
	password?: string;
	autodiscoveryEnabled: boolean;
	autodiscoveryPrefix?: string;
	clientId?: string;
};
