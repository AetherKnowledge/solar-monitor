export type WifiNetwork = {
	ssid: string;
	rssi: number;
	encryptionType: number;
	saved: boolean;
	connected: boolean;
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

export type NetworkConfig = {
	networkMode: 'ap+sta' | 'sta';
	ssid?: string;
	password?: string;
};
