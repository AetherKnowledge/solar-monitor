export type NetworkData = {
	ssid: string;
	signal: 1 | 2 | 3 | 4 | 5;
	secure: boolean;
	saved?: boolean;
	connected?: boolean;
};

export type NetworkConfig = {
	networkMode: 'ap+sta' | 'sta';
	ssid?: string;
	password?: string;
};
