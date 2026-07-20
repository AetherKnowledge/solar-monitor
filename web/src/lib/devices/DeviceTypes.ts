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

export interface RegisterListItem {
	id: string;
	type: RegisterType;

	name: string;
	address?: number;

	value?: string;
	unit?: string;

	data: RegisterItem;
}

export type RegisterItem =
	| {
			register: ReadRegister;
			type: RegisterType.Read;
	  }
	| {
			register: VirtualSensor;
			type: RegisterType.Virtual;
	  }
	| {
			register: SelectWriteRegister;
			type: RegisterType.Select;
	  }
	| {
			register: NumberWriteRegister;
			type: RegisterType.Number;
	  };

export interface DeviceDiscovery {
	name: string;
	identifier: string;
	manufacturer: string;
	model?: string;
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

export const defaultReadRegister: ReadRegister = {
	address: 0,
	rounding: 0,
	transform: RegisterTransform.None,
	transformArgument: 0,
	signedValue: false,
	discovery: {
		name: '',
		unique_id: ''
	}
};

export const defaultVirtualSensor: VirtualSensor = {
	expression: '',
	isPersistent: false,
	discovery: {
		name: '',
		unique_id: ''
	}
};

export const defaultSelectWriteRegister: SelectWriteRegister = {
	address: 0,
	discovery: {
		name: '',
		unique_id: '',
		command_topic: '',
		qos: 0,
		options: []
	}
};

export const defaultNumberWriteRegister: NumberWriteRegister = {
	address: 0,
	discovery: {
		name: '',
		unique_id: '',
		command_topic: '',
		qos: 0,
		min: 0,
		max: 0,
		step: 0
	}
};

export function getDefaultRegister(type: RegisterType): RegisterItem {
	switch (type) {
		case RegisterType.Read:
			return { type: RegisterType.Read, register: defaultReadRegister };
		case RegisterType.Virtual:
			return { type: RegisterType.Virtual, register: defaultVirtualSensor };
		case RegisterType.Select:
			return { type: RegisterType.Select, register: defaultSelectWriteRegister };
		case RegisterType.Number:
			return { type: RegisterType.Number, register: defaultNumberWriteRegister };
	}
}

export function addRegister(device: ModbusDevice, register: RegisterItem) {
	switch (register.type) {
		case RegisterType.Read:
			device.readRegisters.push(register.register);
			return;
		case RegisterType.Virtual:
			device.virtualSensors.push(register.register);
			return;
		case RegisterType.Select:
			device.selectWriteRegisters.push(register.register);
			return;
		case RegisterType.Number:
			device.numberWriteRegisters.push(register.register);
			return;
	}
}

export function removeRegister(device: ModbusDevice, register: RegisterItem) {
	switch (register.type) {
		case RegisterType.Read:
			device.readRegisters = device.readRegisters.filter((r) => r !== register.register);
			return;
		case RegisterType.Virtual:
			device.virtualSensors = device.virtualSensors.filter((r) => r !== register.register);
			return;
		case RegisterType.Select:
			device.selectWriteRegisters = device.selectWriteRegisters.filter(
				(r) => r !== register.register
			);
			return;
		case RegisterType.Number:
			device.numberWriteRegisters = device.numberWriteRegisters.filter(
				(r) => r !== register.register
			);
			return;
	}
}
