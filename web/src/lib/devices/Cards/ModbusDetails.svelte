<script lang="ts">
	import type { ModbusDevice } from '$lib/devices/DeviceTypes';
	import { ArrowLeftRight, Cable, Clock3, Cpu, Hash, Repeat2 } from '@lucide/svelte';

	type Props = {
		device: ModbusDevice;
	};

	const { device = $bindable() }: Props = $props();
</script>

<div class="card bg-base-100 border border-base-300 shadow-sm">
	<div class="card-body gap-6">
		<div class="flex items-center gap-3">
			<div class="bg-primary/10 text-primary rounded-xl p-3">
				<Cpu class="size-6" />
			</div>

			<div>
				<h2 class="card-title">Modbus Settings</h2>
				<p class="text-base-content/60 text-sm">
					Configure how this device communicates over Modbus RTU.
				</p>
			</div>
		</div>

		<div class="grid gap-5 md:grid-cols-2">
			<label class="form-control">
				<div class="label">
					<span class="label-text flex items-center gap-2">
						<Hash class="size-4 text-primary" />
						Slave ID
					</span>
				</div>

				<input
					type="number"
					min="1"
					max="247"
					class="input input-bordered w-full"
					bind:value={device.slaveId}
				/>

				<div class="label">
					<span class="label-text-alt"> Modbus slave address (1–247) </span>
				</div>
			</label>

			<label class="form-control">
				<div class="label">
					<span class="label-text flex items-center gap-2">
						<Cable class="size-4 text-primary" />
						Serial Port
					</span>
				</div>

				<select class="select select-bordered w-full" bind:value={device.port}>
					<option value={1}>Port 1</option>
					<option value={2}>Port 2</option>
				</select>

				<div class="label">
					<span class="label-text-alt"> ESP32 UART interface </span>
				</div>
			</label>

			<label class="form-control">
				<div class="label">
					<span class="label-text flex items-center gap-2">
						<Repeat2 class="size-4 text-primary" />
						Baud Rate
					</span>
				</div>

				<input type="number" class="input input-bordered w-full" bind:value={device.baudrate} />

				<div class="label">
					<span class="label-text-alt"> Communication speed (bps) </span>
				</div>
			</label>

			<label class="form-control">
				<div class="label">
					<span class="label-text flex items-center gap-2">
						<Clock3 class="size-4 text-primary" />
						Timeout
					</span>
				</div>

				<div class="join w-full">
					<input
						type="number"
						min="100"
						class="input input-bordered join-item w-full"
						bind:value={device.timeout}
					/>

					<span class="join-item btn btn-disabled">ms</span>
				</div>

				<div class="label">
					<span class="label-text-alt"> Response timeout before retrying </span>
				</div>
			</label>
		</div>

		<div class="divider my-0"></div>

		<div
			class="flex items-center justify-between rounded-xl border border-base-300 bg-base-200/50 p-4"
		>
			<div class="flex items-start gap-3">
				<div class="rounded-lg bg-primary/10 p-2 text-primary">
					<ArrowLeftRight class="size-5" />
				</div>

				<div>
					<h3 class="font-medium">Swap Bytes</h3>
					<p class="text-base-content/60 text-sm">
						Reverse the byte order when decoding Modbus registers.
					</p>
				</div>
			</div>

			<input
				type="checkbox"
				class="toggle toggle-primary toggle-lg"
				bind:checked={device.swapBytes}
			/>
		</div>
	</div>
</div>
