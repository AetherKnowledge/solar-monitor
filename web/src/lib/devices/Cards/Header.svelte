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

<div class="hero mb-6 w-full rounded-2xl border border-base-300 bg-base-100 shadow-sm">
	<div
		class="hero-content w-full max-w-full flex-col items-start gap-4 p-5 sm:flex-row sm:items-center sm:justify-between"
	>
		<div class="flex w-full items-start gap-4">
			<div class="rounded-2xl bg-primary/10 p-3 sm:p-4">
				<Sun class="size-8 text-primary sm:size-10" />
			</div>

			<div class="min-w-0 flex-1">
				<h1 class="wrap-break-words text-2xl font-bold sm:text-3xl">
					{device.discovery.name}
				</h1>

				<p class="font-mono text-xs break-all text-base-content/60 sm:text-sm">
					{device.discovery.identifier}
				</p>

				<div class="mt-3 flex flex-wrap gap-2">
					<div class="badge gap-1 text-xs badge-primary">
						<Hash class="size-3" />
						Slave {device.slaveId}
					</div>

					<div class="badge gap-1 badge-outline text-xs">
						<Cable class="size-3" />
						Port {device.port}
					</div>

					<div class="badge gap-1 badge-outline text-xs">
						<Cpu class="size-3" />
						{device.baudrate} baud
					</div>

					<div
						class={`badge gap-1 text-xs ${device.mqttEnabled ? 'badge-success' : 'badge-error'}`}
					>
						{#if device.mqttEnabled}
							<Wifi class="size-3" />
							MQTT Enabled
						{:else}
							<WifiOff class="size-3" />
							MQTT Disabled
						{/if}
					</div>
				</div>
			</div>

			<button
				class="btn btn-circle shrink-0 btn-ghost btn-sm sm:hidden"
				onclick={() => goto(resolve('/devices'))}
			>
				<ArrowLeft class="size-4" />
			</button>
		</div>

		<button
			class="btn hidden btn-circle btn-ghost sm:flex"
			onclick={() => goto(resolve('/devices'))}
		>
			<ArrowLeft class="size-5" />
		</button>
	</div>
</div>
