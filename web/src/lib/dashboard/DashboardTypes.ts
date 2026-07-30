export interface SystemSensors {
	uptime: number;
	ram_usage: number;
	ram_total: number;
	ram_largest_free_block: number;
	temperature: number;
	flash_used: number;
	flash_total: number;
	web_storage_used: number;
	web_storage_total: number;
	config_storage_used: number;
	config_storage_total: number;
}

export interface SystemState {
	sensors: SystemSensors;
}

export const defaultSystemState: SystemState = {
	sensors: {
		uptime: 0,
		ram_usage: 0,
		ram_total: 0,
		ram_largest_free_block: 0,
		temperature: 0,
		flash_used: 0,
		flash_total: 0,
		web_storage_used: 0,
		web_storage_total: 0,
		config_storage_used: 0,
		config_storage_total: 0
	}
};
