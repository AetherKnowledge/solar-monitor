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
	class="card bg-base-100 border border-base-300 hover:border-primary hover:shadow-lg transition-all"
>
	<div class="card-body gap-5">
		<div class="flex justify-between items-start">
			<div class="flex gap-4">
				<div class="bg-primary/10 rounded-xl p-4 h-fit">
					<Sun class="size-8 text-primary" />
				</div>

				<div>
					<h2 class="card-title text-xl">{device.discovery.identifier}</h2>

					<p class="text-base-content/60 text-sm font-mono">
						{device.discovery.identifier}
					</p>

					{#if device.discovery.manufacturer || device.discovery.model}
						<p class="text-xs text-base-content/50 mt-1">
							{device.discovery.manufacturer}
							{#if device.discovery.model}
								• {device.discovery.model}
							{/if}
						</p>
					{/if}
				</div>
			</div>

			<div class="flex gap-2">
				<div class="badge badge-primary gap-1">
					<Hash class="size-3.5" />
					ID {device.slaveId}
				</div>

				<div class="badge badge-outline gap-1">
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

		<div class="stats stats-horizontal shadow bg-base-200">
			<div class="stat py-3 px-5">
				<div class="stat-figure text-primary">
					<Gauge class="size-5" />
				</div>
				<div class="stat-value text-lg">
					{device.readRegisters.length}
				</div>
				<div class="stat-desc">Read Registers</div>
			</div>

			<div class="stat py-3 px-5">
				<div class="stat-figure text-secondary">
					<Calculator class="size-5" />
				</div>
				<div class="stat-value text-lg">
					{device.virtualSensors.length}
				</div>
				<div class="stat-desc">Virtual Sensors</div>
			</div>

			<div class="stat py-3 px-5">
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
