<script lang="ts">
	import {
		ArrowLeftRight,
		Binary,
		Cpu,
		FingerprintPattern,
		Hash,
		House,
		Image,
		Package,
		Ruler,
		Settings2,
		Sigma,
		Waypoints
	} from '@lucide/svelte';

	let { register = $bindable() }: { register: ReadRegister } = $props();

	import { RegisterTransform, type ReadRegister } from '../DeviceTypes';
	import DataInput from './DataInput.svelte';
	import DataSelect from './DataSelect.svelte';
</script>

<div class="flex-1 overflow-y-auto p-6">
	<div class="flex items-center gap-3">
		<div class="bg-primary/10 text-primary rounded-xl p-3">
			<Package class="size-6" />
		</div>

		<div>
			<h2 class="card-title">General</h2>
			<p class="text-base-content/60 text-sm">Basic information about this register.</p>
		</div>
	</div>

	<div class="mt-6 grid gap-5 md:grid-cols-2">
		<DataInput
			label="Name"
			description="Friendly name shown throughout the application."
			placeholder="Battery Voltage"
			icon={Package}
			bind:value={register.discovery.name}
		/>
		<DataInput
			label="Identifier"
			description="Unique identifier for the register."
			placeholder="battery_voltage"
			icon={FingerprintPattern}
			bind:value={register.discovery.unique_id}
		/>

		<DataInput
			label="Address"
			description="Modbus register address."
			placeholder="4501"
			type="number"
			icon={Hash}
			bind:value={register.address}
		/>

		<DataInput
			label="Unit"
			description="Measurement unit displayed to users."
			placeholder="V"
			icon={Ruler}
			bind:value={register.discovery.unit_of_measurement}
		/>
	</div>

	<div class="divider my-6"></div>

	<!-- Data Processing -->

	<div class="flex items-center gap-3">
		<div class="bg-secondary/10 text-secondary rounded-xl p-3">
			<Settings2 class="size-6" />
		</div>

		<div>
			<h2 class="card-title">Data Processing</h2>
			<p class="text-base-content/60 text-sm">Configure how the raw Modbus value is interpreted.</p>
		</div>
	</div>

	<div class="mt-6 grid gap-5 md:grid-cols-2">
		<DataSelect
			label="Transform"
			description="Mathematical operation to apply to the raw value."
			icon={ArrowLeftRight}
			iconColor="text-secondary"
			options={Object.values(RegisterTransform).map((t) => t.toString())}
			bind:value={register.transform}
		/>

		<DataInput
			label="Transform Argument"
			description="Value used by the selected transform."
			placeholder="1"
			type="number"
			icon={Binary}
			iconColor="text-secondary"
			bind:value={register.transformArgument}
		/>

		<DataInput
			label="Rounding"
			description="Number of decimal places."
			placeholder="2"
			type="number"
			icon={Waypoints}
			iconColor="text-secondary"
			bind:value={register.rounding}
		/>
	</div>

	<div class="mt-5 rounded-xl border border-base-300 bg-base-200/50 p-4">
		<div class="flex items-center justify-between">
			<div class="flex items-start gap-3">
				<div class="rounded-lg bg-secondary/10 p-2 text-secondary">
					<Sigma class="size-5" />
				</div>

				<div>
					<h3 class="font-medium">Signed Value</h3>

					<p class="text-base-content/60 text-sm">Interpret the register as a signed integer.</p>
				</div>
			</div>

			<input
				type="checkbox"
				class="toggle toggle-secondary toggle-lg"
				bind:checked={register.signedValue}
			/>
		</div>
	</div>

	<div class="divider my-6"></div>

	<!-- Home Assistant -->

	<div class="flex items-center gap-3">
		<div class="bg-success/10 text-success rounded-xl p-3">
			<House class="size-6" />
		</div>

		<div>
			<h2 class="card-title">Home Assistant Discovery</h2>
			<p class="text-base-content/60 text-sm">
				Optional metadata published through MQTT Discovery.
			</p>
		</div>
	</div>

	<div class="mt-6 grid gap-5 md:grid-cols-2">
		<DataInput
			label="Device Class"
			description="Defines the type of sensor for Home Assistant."
			placeholder="voltage"
			icon={Cpu}
			iconColor="text-success"
			bind:value={register.discovery.device_class}
		/>

		<DataInput
			label="State Class"
			description="Defines how the sensor's state is interpreted."
			placeholder="measurement"
			icon={Binary}
			iconColor="text-success"
			bind:value={register.discovery.state_class}
		/>

		<DataInput
			label="Unit of Measurement"
			description="Unit displayed in Home Assistant."
			placeholder="V"
			icon={Image}
			iconColor="text-success"
			bind:value={register.discovery.unit_of_measurement}
		/>

		<DataInput
			label="Icon"
			description="Custom icon for the sensor in Home Assistant."
			placeholder="mdi:flash"
			icon={House}
			iconColor="text-success"
			bind:value={register.discovery.icon}
		/>
	</div>
</div>
