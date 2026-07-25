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
	AP = 'wifi_ap',
	STA = 'wifi_sta',
	AP_STA = 'ap+sta'
}

export type NetworkConfig = {
	mode: WifiMode;
	wifiSsid?: string;
	wifiPassword?: string;
	apSsid?: string;
	apPasswordEnabled?: boolean;
	apPassword?: string;
};

export const defaultNetworkConfig: NetworkConfig = {
	mode: WifiMode.AP_STA,
	wifiSsid: '',
	wifiPassword: '',
	apSsid: '',
	apPassword: '',
	apPasswordEnabled: false
};
