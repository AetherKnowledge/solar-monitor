import type {
	DeviceDiscovery,
	ModbusDevice,
	NumberWriteRegister,
	ReadRegister,
	SelectWriteRegister,
	VirtualSensor
} from './DeviceTypes';

type RegisterEntity = ReadRegister | VirtualSensor | SelectWriteRegister | NumberWriteRegister;

export type Patch<T> = {
	[K in keyof T]?: T[K] extends object ? Patch<T[K]> : T[K];
};

export function createDevicePatch(
	original: ModbusDevice,
	current: ModbusDevice
): Patch<ModbusDevice> | undefined {
	const patch: Patch<ModbusDevice> = {};

	copyChangedFields(original, current, patch, [
		'mqttEnabled',
		'slaveId',
		'timeout',
		'baudrate',
		'port',
		'swapBytes'
	]);

	const discovery = diffDiscovery(original.discovery, current.discovery);
	if (discovery) {
		patch.discovery = discovery;
	}

	const readRegisters = diffEntities(original.readRegisters, current.readRegisters);
	if (readRegisters) {
		patch.readRegisters = readRegisters;
	}

	const virtualSensors = diffEntities(original.virtualSensors, current.virtualSensors);
	if (virtualSensors) {
		patch.virtualSensors = virtualSensors;
	}

	const selectWriteRegisters = diffEntities(
		original.selectWriteRegisters,
		current.selectWriteRegisters
	);
	if (selectWriteRegisters) {
		patch.selectWriteRegisters = selectWriteRegisters;
	}

	const numberWriteRegisters = diffEntities(
		original.numberWriteRegisters,
		current.numberWriteRegisters
	);
	if (numberWriteRegisters) {
		patch.numberWriteRegisters = numberWriteRegisters;
	}

	return Object.keys(patch).length ? patch : undefined;
}

function diffDiscovery(
	original: DeviceDiscovery,
	current: DeviceDiscovery
): Patch<DeviceDiscovery> | undefined {
	const patch: Patch<DeviceDiscovery> = {};

	copyChangedFields(original, current, patch, ['name', 'identifier', 'manufacturer', 'model']);

	return Object.keys(patch).length ? patch : undefined;
}

function diffEntities<T extends RegisterEntity>(
	original: T[],
	current: T[]
): Patch<T>[] | undefined {
	const result: Patch<T>[] = [];

	const map = new Map(original.map((item) => [item.discovery.unique_id, item]));

	for (const item of current) {
		const old = map.get(item.discovery.unique_id);

		if (!old) {
			result.push(structuredClone(item));
			continue;
		}

		const patch = diffEntity(old, item);

		if (patch) {
			//@ts-expect-error TypeScript is not able to infer the type of discovery correctly here, but we know it's safe to cast it to Patch<T['discovery']>
			patch.discovery ??= {};
			patch.discovery.unique_id = item.discovery.unique_id;
			result.push(patch);
		}
	}

	return result.length ? result : undefined;
}

function diffEntity<T extends RegisterEntity>(original: T, current: T): Patch<T> | undefined {
	const patch: Patch<T> = {};

	for (const key in current) {
		if (key === 'value') continue;

		if (key === 'discovery') {
			const discovery = diffObject(original.discovery, current.discovery);

			if (discovery) {
				//@ts-expect-error TypeScript is not able to infer the type of discovery correctly here, but we know it's safe to cast it to Patch<T['discovery']>
				patch.discovery = discovery as Patch<T['discovery']>;
			}

			continue;
		}

		if (Object.is(original[key], current[key])) continue;

		patch[key] = current[key] as Patch<T>[typeof key];
	}

	return Object.keys(patch).length ? patch : undefined;
}

function diffObject<T extends object>(original: T, current: T): Patch<T> | undefined {
	const patch: Patch<T> = {};

	for (const key in current) {
		const oldValue = original[key];
		const newValue = current[key];

		if (Array.isArray(oldValue) && Array.isArray(newValue)) {
			if (
				oldValue.length === newValue.length &&
				oldValue.every((v, i) => Object.is(v, newValue[i]))
			) {
				continue;
			}
		} else if (Object.is(oldValue, newValue)) {
			continue;
		}

		patch[key] = newValue as Patch<T>[typeof key];
	}

	return Object.keys(patch).length ? patch : undefined;
}

function copyChangedFields<T, K extends keyof T>(
	original: T,
	current: T,
	patch: Patch<T>,
	keys: readonly K[]
) {
	for (const key of keys) {
		if (!Object.is(original[key], current[key])) {
			patch[key] = current[key] as Patch<T>[K];
		}
	}
}
