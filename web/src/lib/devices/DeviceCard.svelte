<script lang="ts">
	import type { ModbusDevice } from '$lib/devices/DeviceTypes';
	import { ArrowLeftRight, Cable, Clock3, Cpu, Hash, Sun } from '@lucide/svelte';

	type Props = {
		device: ModbusDevice;
	};

	const { device }: Props = $props();
</script>

<div
	class="card card-side bg-base-100 border border-base-300 hover:border-primary hover:shadow-md transition-all duration-200"
>
	<div class="flex items-center justify-center px-6">
		<div class="bg-primary/10 rounded-xl p-4">
			<Sun class="size-8 text-primary" />
		</div>
	</div>

	<div class="card-body py-5">
		<div class="flex items-start justify-between">
			<div>
				<h2 class="card-title">{device.name}</h2>
				<p class="text-base-content/60 text-sm">
					{device.identifier}
				</p>
			</div>

			<div class="flex gap-2">
				<div class="badge badge-primary gap-1">
					<Hash class="size-3.5" />
					Slave {device.slaveId}
				</div>

				<div class="badge badge-outline gap-1">
					<Cable class="size-3.5" />
					Port {device.port}
				</div>
			</div>
		</div>

		<div class="mt-2 flex flex-wrap gap-x-5 gap-y-2 text-sm text-base-content/70">
			<div class="flex items-center gap-1.5">
				<Cpu class="size-4" />
				<span>{device.baudrate.toLocaleString()} baud</span>
			</div>

			<div class="flex items-center gap-1.5">
				<ArrowLeftRight class="size-4" />
				<span>{device.swapBytes ? 'Swap Bytes' : 'No Swap'}</span>
			</div>

			<div class="flex items-center gap-1.5">
				<Clock3 class="size-4" />
				<span>{device.timeout} ms timeout</span>
			</div>
		</div>

		<div class="card-actions justify-between items-center mt-2">
			<div class="badge badge-accent">
				{device.readRegisters.length} Registers
			</div>

			<button class="btn btn-sm btn-primary"> Configure </button>
		</div>
	</div>
</div>
