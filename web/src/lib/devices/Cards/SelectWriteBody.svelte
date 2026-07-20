<script lang="ts">
	import {
		Cpu,
		FingerprintPattern,
		Hash,
		House,
		Image,
		Package,
		Settings2,
		TextCursorInput,
		Waypoints
	} from '@lucide/svelte';

	import type { SelectWriteRegister } from '../DeviceTypes';

	import DataInput from './DataInput.svelte';
	import OptionListEditor from './OptionListEditor.svelte';

	let { register = $bindable() }: { register: SelectWriteRegister } = $props();
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
				Basic information about this writable select register.
			</p>
		</div>
	</div>

	<div class="mt-6 grid gap-5 md:grid-cols-2">
		<DataInput
			label="Name"
			description="Friendly name shown throughout the application."
			placeholder="Charge Mode"
			icon={Package}
			bind:value={register.discovery.name}
		/>

		<DataInput
			label="Identifier"
			description="Unique identifier for the register."
			placeholder="charge_mode"
			icon={FingerprintPattern}
			bind:value={register.discovery.unique_id}
		/>

		<DataInput
			label="Address"
			description="Modbus register address."
			type="number"
			placeholder="4600"
			icon={Hash}
			bind:value={register.address}
		/>

		<DataInput
			label="QoS"
			description="MQTT Quality of Service level."
			type="number"
			placeholder="0"
			icon={Waypoints}
			bind:value={register.discovery.qos}
		/>
	</div>

	<div class="divider my-6"></div>

	<!-- Select Configuration -->

	<div class="flex items-center gap-3">
		<div class="bg-secondary/10 text-secondary rounded-xl p-3">
			<Settings2 class="size-6" />
		</div>

		<div>
			<h2 class="card-title">Select Configuration</h2>
			<p class="text-base-content/60 text-sm">Configure the available options and MQTT behavior.</p>
		</div>
	</div>

	<div class="my-6 grid gap-5 md:grid-cols-2">
		<DataInput
			label="Command Topic"
			description="MQTT topic used for writes."
			placeholder="homeassistant/select/..."
			icon={House}
			iconColor="text-secondary"
			bind:value={register.discovery.command_topic}
		/>

		<DataInput
			label="Command Template"
			description="Optional payload template."
			placeholder={`"{{ value }}"`}
			icon={TextCursorInput}
			iconColor="text-secondary"
			bind:value={register.discovery.command_template}
		/>
	</div>

	<OptionListEditor
		label="Options"
		description="Available values users can choose from."
		placeholder="Solar First"
		bind:options={register.discovery.options}
	/>

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
			description="Defines the select entity type."
			placeholder="enum"
			icon={Cpu}
			iconColor="text-success"
			bind:value={register.discovery.device_class}
		/>

		<DataInput
			label="Icon"
			description="Custom icon displayed in Home Assistant."
			placeholder="mdi:format-list-bulleted"
			icon={Image}
			iconColor="text-success"
			bind:value={register.discovery.icon}
		/>

		<DataInput
			label="Value Template"
			description="Optional MQTT value template."
			placeholder={`"{{ value }}"`}
			icon={TextCursorInput}
			iconColor="text-success"
			bind:value={register.discovery.value_template}
		/>
	</div>
</div>
