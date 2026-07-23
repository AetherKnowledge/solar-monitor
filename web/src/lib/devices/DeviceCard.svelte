<script lang="ts">
	import { resolve } from '$app/paths';
	import type { ModbusDevice } from '$lib/devices/DeviceTypes';
	import {
		ArrowLeftRight,
		Cable,
		Calculator,
		Clock3,
		Cpu,
		Gauge,
		Hash,
		SlidersHorizontal,
		Sun,
		Wifi,
		WifiOff
	} from '@lucide/svelte';

	type Props = {
		device: ModbusDevice;
	};

	const { device = $bindable() }: Props = $props();
</script>

<div
	class="card border border-base-300 bg-base-100 transition-all hover:border-primary hover:shadow-lg"
>
	<div class="card-body gap-5">
		<div class="flex items-start justify-between">
			<div class="flex gap-4">
				<div class="h-fit rounded-xl bg-primary/10 p-4">
					<Sun class="size-8 text-primary" />
				</div>

				<div>
					<h2 class="card-title text-xl">{device.discovery.name}</h2>

					<p class="font-mono text-sm text-base-content/60">
						{device.discovery.identifier}
					</p>

					{#if device.discovery.manufacturer || device.discovery.model}
						<p class="mt-1 text-xs text-base-content/50">
							{device.discovery.manufacturer}
							{#if device.discovery.model}
								• {device.discovery.model}
							{/if}
						</p>
					{/if}
				</div>
			</div>

			<div class="flex gap-2">
				<div class="badge gap-1 badge-primary">
					<Hash class="size-3.5" />
					ID {device.slaveId}
				</div>

				<div class="badge gap-1 badge-outline">
					<Cable class="size-3.5" />
					Port {device.port}
				</div>

				<div class={`badge ${device.mqttEnabled ? 'badge-success' : 'badge-error'} gap-1`}>
					{#if device.mqttEnabled}
						<Wifi class="size-3.5" />
						MQTT
					{:else}
						<WifiOff class="size-3.5" />
						Disabled
					{/if}
				</div>
			</div>
		</div>

		<div class="grid grid-cols-3 gap-3 text-sm">
			<div class="flex items-center gap-2">
				<Cpu class="size-4 text-primary" />
				<span>{device.baudrate.toLocaleString()} baud</span>
			</div>

			<div class="flex items-center gap-2">
				<Clock3 class="size-4 text-primary" />
				<span>{device.timeout} ms</span>
			</div>

			<div class="flex items-center gap-2">
				<ArrowLeftRight class="size-4 text-primary" />
				<span>{device.swapBytes ? 'Swap Bytes' : 'Normal Bytes'}</span>
			</div>
		</div>

		<div class="divider my-0"></div>

		<div class="stats stats-horizontal bg-base-200 shadow">
			<div class="stat px-5 py-3">
				<div class="stat-figure text-primary">
					<Gauge class="size-5" />
				</div>
				<div class="stat-value text-lg">
					{device.readRegisters.length}
				</div>
				<div class="stat-desc">Read Registers</div>
			</div>

			<div class="stat px-5 py-3">
				<div class="stat-figure text-secondary">
					<Calculator class="size-5" />
				</div>
				<div class="stat-value text-lg">
					{device.virtualSensors.length}
				</div>
				<div class="stat-desc">Virtual Sensors</div>
			</div>

			<div class="stat px-5 py-3">
				<div class="stat-figure text-accent">
					<SlidersHorizontal class="size-5" />
				</div>
				<div class="stat-value text-lg">
					{device.selectWriteRegisters.length + device.numberWriteRegisters.length}
				</div>
				<div class="stat-desc">Write Registers</div>
			</div>
		</div>

		<div class="card-actions justify-end">
			<a href={resolve(`/devices/${device.discovery.identifier}/`)} class="btn btn-primary">
				Configure
			</a>
		</div>
	</div>
</div>
