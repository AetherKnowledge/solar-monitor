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
	<div class="card-body gap-4 sm:gap-5">
		<!-- Header -->
		<div class="flex items-start gap-3">
			<div class="rounded-xl bg-primary/10 p-3 sm:p-4">
				<Sun class="size-7 text-primary sm:size-8" />
			</div>

			<div class="min-w-0 flex-1">
				<h2 class="card-title text-xl leading-tight">
					{device.discovery.name}
				</h2>

				<p class="mt-1 font-mono text-xs break-all text-base-content/60">
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

				<div class="mt-3 flex flex-wrap gap-2">
					<div class="badge gap-1 badge-sm badge-primary">
						<Hash class="size-3" />
						Slave {device.slaveId}
					</div>

					<div class="badge gap-1 badge-outline badge-sm">
						<Cable class="size-3" />
						Port {device.port}
					</div>

					<div class="badge gap-1 badge-outline badge-sm">
						<Cpu class="size-3" />
						{device.baudrate}
						baud
					</div>

					<div
						class={`badge gap-1 badge-sm ${device.mqttEnabled ? 'badge-success' : 'badge-error'}`}
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
		</div>
		<!-- Device Info -->
		<div class="grid grid-cols-1 gap-3 text-xs sm:grid-cols-3 sm:text-sm">
			<div class="flex items-center gap-2">
				<Cpu class="size-4 shrink-0 text-primary" />
				<span>{device.baudrate.toLocaleString()} baud</span>
			</div>

			<div class="flex items-center gap-2">
				<Clock3 class="size-4 shrink-0 text-primary" />
				<span>{device.timeout} ms</span>
			</div>

			<div class="flex items-center gap-2">
				<ArrowLeftRight class="size-4 shrink-0 text-primary" />
				<span>{device.swapBytes ? 'Swap Bytes' : 'Normal Bytes'}</span>
			</div>
		</div>

		<div class="divider my-0"></div>

		<!-- Stats -->
		<div class="stats stats-vertical bg-base-200 shadow sm:stats-horizontal">
			<div class="stat px-4 py-3">
				<div class="stat-figure text-primary">
					<Gauge class="size-5" />
				</div>

				<div class="stat-value text-lg">
					{device.readRegisters.length}
				</div>

				<div class="stat-desc">Read Registers</div>
			</div>

			<div class="stat px-4 py-3">
				<div class="stat-figure text-secondary">
					<Calculator class="size-5" />
				</div>

				<div class="stat-value text-lg">
					{device.virtualSensors.length}
				</div>

				<div class="stat-desc">Virtual Sensors</div>
			</div>

			<div class="stat px-4 py-3">
				<div class="stat-figure text-accent">
					<SlidersHorizontal class="size-5" />
				</div>

				<div class="stat-value text-lg">
					{device.selectWriteRegisters.length + device.numberWriteRegisters.length}
				</div>

				<div class="stat-desc">Write Registers</div>
			</div>
		</div>

		<!-- Actions -->
		<div class="card-actions justify-stretch sm:justify-end">
			<a
				href={resolve(`/devices/${device.discovery.identifier}/`)}
				class="btn w-full btn-primary btn-sm sm:w-auto"
			>
				Configure
			</a>
		</div>
	</div>
</div>
