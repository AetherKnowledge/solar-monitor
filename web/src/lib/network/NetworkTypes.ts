import type { UpdateStatus } from '$lib/common/CommonTypes';

export type WifiNetwork = {
	ssid: string;
	rssi: number;
	encryptionType: number;
	saved: boolean;
};

export type ScanRequestResponse = {
	status: UpdateStatus;
	networks: WifiNetwork[];
};

export enum WifiMode {
	AP = 'ap',
	STA = 'sta',
	AP_STA = 'ap+sta'
}

export type NetworkConfig = {
	mode: WifiMode;
	ssid?: string;
	password?: string;
};

export const defaultNetworkConfig: NetworkConfig = {
	mode: WifiMode.AP_STA,
	ssid: '',
	password: ''
};
