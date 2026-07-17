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

	discoveryConfig: MqttDiscoveryConfig;
}

export interface ModbusDevice {
	name: string;
	identifier: string;
	slaveId: number;
	timeout: number;
	baudrate: number;
	port: number;
	wordOrder: WordOrder;

	discoveryDevice: MqttDiscoveryDevice;
	readRegisters: ReadRegister[];
}

export interface MqttDiscoveryDevice {
	identifier: string;
	manufacturer: string;
	model: string;
	name: string;
}

export interface MqttDiscoveryConfig {
	name: string;
	uniqueId: string;
	stateTopic: string;

	deviceClass: string;
	stateClass: string;
	unitOfMeasurement: string;
	icon: string;
}
