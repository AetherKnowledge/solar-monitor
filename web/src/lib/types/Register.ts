export type RegisterMap = {
	id: number;
	name: string;
	registers: Register[];
};

export type Register = ReadRegister | WriteRegister;

export type ReadRegister = {
	id: number;
	type: 'read';
	name: string;
	topic: string;
	transform: 'divide';
	unitOfMeasurement?: string;
	deviceClass?: string;
	stateClass?: string;
	argument: number;
	rounding: number;
	autoDiscovery: boolean;
};

export type WriteRegister = {
	id: number;
	name: string;
	commandTopic: string;
	commandTemplate: string;
	valueTemplate?: string;
	qos: number;
};
