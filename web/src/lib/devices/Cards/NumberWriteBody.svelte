<script lang="ts">
	import {
		Calculator,
		Cpu,
		FingerprintPattern,
		Hash,
		House,
		Image,
		Package,
		Ruler,
		Settings2,
		SlidersHorizontal,
		Waypoints
	} from '@lucide/svelte';

	import type { NumberWriteRegister } from '../DeviceTypes';

	import DataInput from './DataInput.svelte';

	let { register = $bindable() }: { register: NumberWriteRegister } = $props();
</script>

<div class="flex-1 overflow-y-auto p-6">
	<!-- General -->

	<div class="flex items-center gap-3">
		<div class="bg-primary/10 text-primary rounded-xl p-3">
			<Package class="size-6" />
		</div>

		<div>
			<h2 class="card-title">General</h2>
			<p class="text-base-content/60 text-sm">
				Basic information about this writable number register.
			</p>
		</div>
	</div>

	<div class="mt-6 grid gap-5 md:grid-cols-2">
		<DataInput
			label="Name"
			description="Friendly name shown throughout the application."
			placeholder="Maximum Charge Current"
			icon={Package}
			bind:value={register.discovery.name}
		/>

		<DataInput
			label="Identifier"
			description="Unique identifier for the register."
			placeholder="max_charge_current"
			icon={FingerprintPattern}
			bind:value={register.discovery.unique_id}
		/>

		<DataInput
			label="Address"
			description="Modbus register address."
			placeholder="4700"
			type="number"
			icon={Hash}
			bind:value={register.address}
		/>

		<DataInput
			label="Unit"
			description="Measurement unit displayed to users."
			placeholder="A"
			icon={Ruler}
			bind:value={register.discovery.unit_of_measurement}
		/>
	</div>

	<div class="divider my-6"></div>

	<!-- Number Configuration -->

	<div class="flex items-center gap-3">
		<div class="bg-secondary/10 text-secondary rounded-xl p-3">
			<Settings2 class="size-6" />
		</div>

		<div>
			<h2 class="card-title">Number Configuration</h2>
			<p class="text-base-content/60 text-sm">
				Configure the writable numeric range and MQTT behavior.
			</p>
		</div>
	</div>

	<div class="mt-6 grid gap-5 md:grid-cols-2">
		<DataInput
			label="Minimum"
			description="Lowest value that can be written."
			type="number"
			placeholder="0"
			icon={SlidersHorizontal}
			iconColor="text-secondary"
			bind:value={register.discovery.min}
		/>

		<DataInput
			label="Maximum"
			description="Highest value that can be written."
			type="number"
			placeholder="100"
			icon={SlidersHorizontal}
			iconColor="text-secondary"
			bind:value={register.discovery.max}
		/>

		<DataInput
			label="Step"
			description="Increment between allowed values."
			type="number"
			placeholder="1"
			icon={Waypoints}
			iconColor="text-secondary"
			bind:value={register.discovery.step}
		/>

		<DataInput
			label="QoS"
			description="MQTT Quality of Service level."
			type="number"
			placeholder="0"
			icon={Calculator}
			iconColor="text-secondary"
			bind:value={register.discovery.qos}
		/>

		<DataInput
			label="Command Topic"
			description="MQTT topic used for writes."
			placeholder="homeassistant/number/..."
			icon={House}
			iconColor="text-secondary"
			bind:value={register.discovery.command_topic}
		/>

		<DataInput
			label="Command Template"
			description="Optional payload template."
			placeholder={`"{{ value }}"`}
			icon={Image}
			iconColor="text-secondary"
			bind:value={register.discovery.command_template}
		/>
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
			description="Defines the number entity type."
			placeholder="current"
			icon={Cpu}
			iconColor="text-success"
			bind:value={register.discovery.device_class}
		/>

		<DataInput
			label="Unit of Measurement"
			description="Unit displayed in Home Assistant."
			placeholder="A"
			icon={Ruler}
			iconColor="text-success"
			bind:value={register.discovery.unit_of_measurement}
		/>

		<DataInput
			label="Icon"
			description="Custom icon displayed in Home Assistant."
			placeholder="mdi:current-ac"
			icon={Image}
			iconColor="text-success"
			bind:value={register.discovery.icon}
		/>

		<DataInput
			label="Value Template"
			description="Optional MQTT value template."
			placeholder={`"{{ value_json.value }}"`}
			icon={Calculator}
			iconColor="text-success"
			bind:value={register.discovery.value_template}
		/>
	</div>
</div>
