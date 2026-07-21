<script lang="ts">
	import {
		Calculator,
		Cpu,
		FingerprintPattern,
		FunctionSquare,
		House,
		Image,
		Package,
		Ruler,
		Sigma,
		TextCursorInput,
		Waypoints
	} from '@lucide/svelte';

	import type { VirtualSensor } from '../DeviceTypes';

	import DataInput from './DataInput.svelte';

	let { register = $bindable() }: { register: VirtualSensor } = $props();
</script>

<div class="flex-1 overflow-y-auto p-6">
	<!-- General -->

	<div class="flex items-center gap-3">
		<div class="rounded-xl bg-primary/10 p-3 text-primary">
			<Package class="size-6" />
		</div>

		<div>
			<h2 class="card-title">General</h2>
			<p class="text-sm text-base-content/60">Basic information about this virtual sensor.</p>
		</div>
	</div>

	<div class="mt-6 grid gap-5 md:grid-cols-2">
		<DataInput
			label="Name"
			description="Friendly name shown throughout the application."
			placeholder="Battery Power"
			icon={Package}
			bind:value={register.discovery.name}
		/>

		<DataInput
			label="Identifier"
			description="Unique identifier for the virtual sensor."
			placeholder="battery_power"
			icon={FingerprintPattern}
			bind:value={register.discovery.unique_id}
		/>

		<DataInput
			label="Unit"
			description="Measurement unit displayed to users."
			placeholder="W"
			icon={Ruler}
			bind:value={register.discovery.unit_of_measurement}
		/>
	</div>

	<div class="divider my-6"></div>

	<!-- Expression -->

	<div class="flex items-center gap-3">
		<div class="rounded-xl bg-secondary/10 p-3 text-secondary">
			<FunctionSquare class="size-6" />
		</div>

		<div>
			<h2 class="card-title">Expression</h2>
			<p class="text-sm text-base-content/60">
				Create a calculated sensor using other register values.
			</p>
		</div>
	</div>

	<div class="my-6">
		<label class="form-control">
			<div class="label">
				<span class="flex items-center gap-1 text-sm">
					<TextCursorInput class="size-4 text-secondary" />
					<p class="pt-0.5">Expression</p>
				</span>
			</div>

			<textarea
				class="textarea-bordered textarea h-28 w-full font-mono"
				placeholder="(battery_voltage * battery_current)"
				bind:value={register.expression}></textarea>

			<div class="label">
				<span class="label-text-alt text-xs">
					Use register identifiers and mathematical operators to calculate a value.
				</span>
			</div>
		</label>
	</div>

	<DataInput
		label="Rounding"
		description="Number of decimal places."
		placeholder="2"
		type="number"
		icon={Waypoints}
		iconColor="text-secondary"
		bind:value={register.rounding}
	/>

	<div class="mt-5 rounded-xl border border-base-300 bg-base-200/50 p-4">
		<div class="flex items-center justify-between">
			<div class="flex items-start gap-3">
				<div class="rounded-lg bg-secondary/10 p-2 text-secondary">
					<Calculator class="size-6" />
				</div>

				<div>
					<h3 class="text-sm font-medium">Persistent Value</h3>

					<p class="text-sm text-base-content/60">
						Restore the last calculated value after the device restarts.
					</p>
				</div>
			</div>

			<input type="checkbox" class="toggle toggle-secondary" bind:checked={register.isPersistent} />
		</div>
	</div>

	<div class="divider my-6"></div>

	<!-- Home Assistant -->

	<div class="flex items-center gap-3">
		<div class="rounded-xl bg-success/10 p-3 text-success">
			<House class="size-6" />
		</div>

		<div>
			<h2 class="card-title">Home Assistant Discovery</h2>
			<p class="text-sm text-base-content/60">
				Optional metadata published through MQTT Discovery.
			</p>
		</div>
	</div>

	<div class="mt-6 grid gap-5 md:grid-cols-2">
		<DataInput
			label="Device Class"
			description="Defines the type of sensor for Home Assistant."
			placeholder="power"
			icon={Cpu}
			iconColor="text-success"
			bind:value={register.discovery.device_class}
		/>

		<DataInput
			label="State Class"
			description="Defines how the sensor state is interpreted."
			placeholder="measurement"
			icon={Sigma}
			iconColor="text-success"
			bind:value={register.discovery.state_class}
		/>

		<DataInput
			label="Unit of Measurement"
			description="Unit displayed in Home Assistant."
			placeholder="W"
			icon={Ruler}
			iconColor="text-success"
			bind:value={register.discovery.unit_of_measurement}
		/>

		<DataInput
			label="Icon"
			description="Custom icon for Home Assistant."
			placeholder="mdi:flash"
			icon={Image}
			iconColor="text-success"
			bind:value={register.discovery.icon}
		/>
	</div>
</div>
