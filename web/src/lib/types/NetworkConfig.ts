export type WifiNetwork = {
	ssid: string;
	rssi: number;
	encryptionType: number;
	saved: boolean;
	connected: boolean;
};

export type NetworkConfig = {
	networkMode: 'ap+sta' | 'sta';
	ssid?: string;
	password?: string;
};
