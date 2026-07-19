export enum RegisterTransform {
	None = 'None',
	Add = 'Add',
	Subtract = 'Subtract',
	Multiply = 'Multiply',
	Divide = 'Divide'
}

export enum WordOrder {
	ABCD = 'ABCD',
	BADC = 'BADC',
	CDAB = 'CDAB',
	DCBA = 'DCBA'
}

export interface ReadRegister {
	address: number;
	rounding: number;
	transform: RegisterTransform;
	transformArgument: number;
	signedValue: boolean;

	discovery: MqttSensorDiscoveryConfig;
}

export interface ModbusDevice {
	name: string;
	identifier: string;
	slaveId: number;
	timeout: number;
	baudrate: number;
	port: number;
	swapBytes: boolean;
	mqttEnabled: boolean;

	discovery: MqttDiscoveryDevice;
	readRegisters: ReadRegister[];
}

export interface MqttDiscoveryDevice {
	identifier: string;
	manufacturer: string;
	model: string;
	name: string;
}

export interface MqttSensorDiscoveryConfig {
	name: string;
	uniqueId: string;

	deviceClass: string;
	stateClass: string;
	unitOfMeasurement: string;
	icon: string;
}
