<script lang="ts">
	import { goto } from '$app/navigation';
	import { resolve } from '$app/paths';
	import type { ModbusDevice } from '$lib/devices/DeviceTypes';
	import { ArrowLeft, Cable, Cpu, Hash, Sun, Wifi, WifiOff } from '@lucide/svelte';

	type Props = {
		device: ModbusDevice;
	};

	const { device }: Props = $props();
</script>

<div class="hero bg-base-100 border border-base-300 rounded-2xl shadow-sm mb-6 w-full">
	<div class="hero-content w-full max-w-full justify-between">
		<div class="flex items-center gap-5">
			<div class="bg-primary/10 rounded-2xl p-4">
				<Sun class="size-10 text-primary" />
			</div>

			<div>
				<h1 class="text-3xl font-bold">
					{device.name}
				</h1>

				<p class="font-mono text-sm text-base-content/60">
					{device.identifier}
				</p>

				<div class="mt-3 flex flex-wrap gap-2">
					<div class="badge badge-primary gap-1">
						<Hash class="size-3.5" />
						Slave {device.slaveId}
					</div>

					<div class="badge badge-outline gap-1">
						<Cable class="size-3.5" />
						Port {device.port}
					</div>

					<div class="badge badge-outline gap-1">
						<Cpu class="size-3.5" />
						{device.baudrate} baud
					</div>

					<div class={`badge ${device.mqttEnabled ? 'badge-success' : 'badge-error'} gap-1`}>
						{#if device.mqttEnabled}
							<Wifi class="size-3.5" />
							MQTT Enabled
						{:else}
							<WifiOff class="size-3.5" />
							MQTT Disabled
						{/if}
					</div>
				</div>
			</div>
		</div>
		<button class="btn btn-circle btn-ghost" onclick={() => goto(resolve('/devices'))}>
			<ArrowLeft class="size-5" />
		</button>
	</div>
</div>
