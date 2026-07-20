<script lang="ts">
	import type { ModbusDevice } from '$lib/devices/DeviceTypes';
	import { Box, Factory, FingerprintPattern, MessageSquare, Package } from '@lucide/svelte';

	type Props = {
		device: ModbusDevice;
	};

	const { device = $bindable() }: Props = $props();
</script>

<div class="card bg-base-100 border border-base-300 shadow-sm">
	<div class="card-body gap-6">
		<div class="flex items-center gap-3">
			<div class="bg-primary/10 text-primary rounded-xl p-3">
				<Box class="size-6" />
			</div>

			<div>
				<h2 class="card-title">General</h2>
				<p class="text-base-content/60 text-sm">
					Basic information and MQTT settings for this device.
				</p>
			</div>
		</div>

		<div class="grid gap-5 md:grid-cols-2">
			<label class="form-control">
				<div class="label">
					<span class="label-text flex items-center gap-2">
						<Package class="size-4 text-primary" />
						Name
					</span>
				</div>

				<input
					class="input input-bordered w-full"
					bind:value={device.discovery.name}
					placeholder="Solar Inverter"
				/>

				<div class="label">
					<span class="label-text-alt"> Friendly name shown throughout the application. </span>
				</div>
			</label>

			<label class="form-control">
				<div class="label">
					<span class="label-text flex items-center gap-2">
						<FingerprintPattern class="size-4 text-primary" />
						Identifier
					</span>
				</div>

				<input class="input input-bordered w-full font-mono" value={device.discovery.identifier} />

				<div class="label">
					<span class="label-text-alt"> Unique identifier for the device. </span>
				</div>
			</label>

			<label class="form-control">
				<div class="label">
					<span class="label-text flex items-center gap-2">
						<Factory class="size-4 text-primary" />
						Manufacturer
					</span>
				</div>

				<input
					class="input input-bordered w-full"
					bind:value={device.discovery.manufacturer}
					placeholder="Y&H"
				/>

				<div class="label">
					<span class="label-text-alt"> Device manufacturer. </span>
				</div>
			</label>

			<label class="form-control">
				<div class="label">
					<span class="label-text flex items-center gap-2">
						<Box class="size-4 text-primary" />
						Model
					</span>
				</div>

				<input
					class="input input-bordered w-full"
					bind:value={device.discovery.model}
					placeholder="YH4K2"
				/>

				<div class="label">
					<span class="label-text-alt"> Model reported to Home Assistant. </span>
				</div>
			</label>
		</div>

		<div class="divider my-0"></div>

		<div
			class="flex items-center justify-between rounded-xl border border-base-300 bg-base-200/50 p-4"
		>
			<div class="flex items-start gap-3">
				<div class="rounded-lg bg-success/10 p-2 text-success">
					<MessageSquare class="size-5" />
				</div>

				<div>
					<h3 class="font-medium">MQTT Integration</h3>
					<p class="text-base-content/60 text-sm">
						Enable publishing sensors and accepting commands through MQTT.
					</p>
				</div>
			</div>

			<input
				type="checkbox"
				class="toggle toggle-success toggle-lg"
				bind:checked={device.mqttEnabled}
			/>
		</div>
	</div>
</div>
