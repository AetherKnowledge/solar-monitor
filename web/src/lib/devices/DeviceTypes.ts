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

export enum RegisterType {
	Read = 'Read',
	Virtual = 'Virtual',
	Select = 'Select',
	Number = 'Number'
}

export interface DeviceDiscovery {
	manufacturer: string;
	model?: string;
	name?: string;
}

export interface SensorDiscovery {
	name: string;
	unique_id: string;

	state_class?: string;
	state_topic?: string;
	unit_of_measurement?: string;
	device_class?: string;
	icon?: string;
}

export interface WriteDiscovery extends SensorDiscovery {
	value_template?: string;
	command_template?: string;
	command_topic: string;
	qos: number;
}

export interface SelectDiscovery extends WriteDiscovery {
	options: string[];
}

export interface NumberDiscovery extends WriteDiscovery {
	min: number;
	max: number;
	step: number;
}

export interface ReadRegister {
	address: number;
	rounding: number;
	transform: RegisterTransform;
	transformArgument: number;
	signedValue: boolean;

	discovery: SensorDiscovery;
}

export interface VirtualSensor {
	expression: string;
	isPersistent: boolean;

	discovery: SensorDiscovery;
}

export interface SelectWriteRegister {
	address: number;
	discovery: SelectDiscovery;
}

export interface NumberWriteRegister {
	address: number;
	discovery: NumberDiscovery;
}

export interface ModbusDevice {
	name: string;
	identifier: string;
	mqttEnabled: boolean;

	slaveId: number;
	timeout: number;
	baudrate: number;
	port: number;
	swapBytes: boolean;

	discovery: DeviceDiscovery;

	readRegisters: ReadRegister[];
	virtualSensors: VirtualSensor[];
	selectWriteRegisters: SelectWriteRegister[];
	numberWriteRegisters: NumberWriteRegister[];
}
