export type WifiNetwork = {
	ssid: string;
	rssi: number;
	encryptionType: number;
	saved: boolean;
};

export enum WifiScanStatus {
	SCAN_STATUS_IN_PROGRESS = 'in_progress',
	SCAN_STATUS_COMPLETE = 'complete',
	SCAN_STATUS_FAILED = 'failed',
	SCAN_STATUS_NOT_STARTED = 'not_started'
}

export type ScanRequestResponse = {
	status: WifiScanStatus;
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
