<script lang="ts">
	import { HardDrive, MemoryStick, MonitorSmartphone, Thermometer, Timer } from '@lucide/svelte';

	import { createDashboardController } from './DashboardController.svelte';

	const { query } = createDashboardController();

	const sensors = $derived(query.data.sensors);

	function percent(used: number, total: number) {
		if (total <= 0) return 0;

		const value = (used / total) * 100;
		return Number.isFinite(value) ? value : 0;
	}

	const ramPercent = $derived(percent(sensors.ram_usage, sensors.ram_total));
	const flashPercent = $derived(percent(sensors.flash_used, sensors.flash_total));
	const codePercent = $derived(percent(sensors.code_storage_used, sensors.code_storage_total));
	const webPercent = $derived(percent(sensors.web_storage_used, sensors.web_storage_total));
	const configPercent = $derived(
		percent(sensors.config_storage_used, sensors.config_storage_total)
	);

	function formatUptime(seconds: number) {
		const days = Math.floor(seconds / 86400);
		seconds %= 86400;

		const hours = Math.floor(seconds / 3600);
		seconds %= 3600;

		const minutes = Math.floor(seconds / 60);

		if (days > 0) return `${days}d ${hours}h`;
		if (hours > 0) return `${hours}h ${minutes}m`;
		return `${minutes}m`;
	}

	function getHealth(percent: number) {
		if (percent < 60) {
			return {
				text: 'Healthy',
				class: 'badge-success'
			};
		}

		if (percent < 85) {
			return {
				text: 'Warning',
				class: 'badge-warning'
			};
		}

		return {
			text: 'Critical',
			class: 'badge-error'
		};
	}

	const ramHealth = $derived(getHealth(ramPercent));
</script>

<div class="card border border-base-300 bg-base-100 shadow-sm">
	<div class="card-body gap-6">
		<!-- Header -->

		<div class="flex items-start justify-between">
			<div class="flex items-center gap-3">
				<div class="rounded-xl bg-primary/10 p-3 text-primary">
					<MonitorSmartphone class="size-7" />
				</div>

				<div>
					<h2 class="card-title">System Dashboard</h2>

					<p class="text-sm text-base-content/60">
						Monitor the health and resource usage of your Solar Monitor device.
					</p>
				</div>
			</div>
		</div>

		<div class="divider my-0"></div>

		<!-- System -->

		<div class="flex items-center gap-3">
			<div class="rounded-xl bg-primary/10 p-3 text-primary">
				<Timer class="size-6" />
			</div>

			<div>
				<h2 class="card-title">System Status</h2>

				<p class="text-sm text-base-content/60">Current runtime information.</p>
			</div>
		</div>

		<div class="grid gap-5 md:grid-cols-2">
			<div class="rounded-xl border border-base-300 bg-base-200/40 p-5">
				<div class="flex items-center gap-3">
					<div class="rounded-lg bg-primary/10 p-2 text-primary">
						<Timer class="size-5" />
					</div>

					<div>
						<div class="text-sm text-base-content/60">System Uptime</div>

						<div class="text-xl font-semibold">
							{formatUptime(sensors.uptime)}
						</div>

						<div class="text-sm text-base-content/60">
							{Math.round(sensors.uptime)} seconds
						</div>
					</div>
				</div>
			</div>

			<div class="rounded-xl border border-base-300 bg-base-200/40 p-5">
				<div class="flex items-center gap-3">
					<div class="rounded-lg bg-error/10 p-2 text-error">
						<Thermometer class="size-5" />
					</div>

					<div>
						<div class="text-sm text-base-content/60">Temperature</div>

						<div class="text-xl font-semibold">
							{sensors.temperature.toFixed(1)} °C
						</div>

						<div class="text-sm text-base-content/60">ESP32 Internal Sensor</div>
					</div>
				</div>
			</div>
		</div>

		<div class="divider my-0"></div>

		<!-- Memory -->

		<div class="flex items-center gap-3">
			<div class="rounded-xl bg-secondary/10 p-3 text-secondary">
				<MemoryStick class="size-6" />
			</div>

			<div>
				<h2 class="card-title">Memory</h2>

				<p class="text-sm text-base-content/60">Current RAM usage.</p>
			</div>
		</div>

		<div class="rounded-xl border border-base-300 bg-base-200/40 p-5">
			<div class="mb-2 flex items-center justify-between">
				<div>
					<div class="font-medium">RAM Usage</div>

					<div class="text-sm text-base-content/60">
						{sensors.ram_usage.toFixed(1)}
						/
						{sensors.ram_total.toFixed(1)}
						KiB
					</div>
				</div>

				<div class={`badge gap-1 ${ramHealth.class}`}>
					<span class="size-2 rounded-full bg-current"></span>
					{ramHealth.text}
				</div>
			</div>

			<progress class="progress w-full progress-primary" value={ramPercent} max="100"></progress>

			<div class="mt-2 flex justify-end text-sm text-base-content/60">
				{ramPercent.toFixed(0)}%
			</div>

			<div class="divider"></div>

			<div class="flex justify-between">
				<span class="text-base-content/70"> Largest Free Block </span>

				<span class="font-mono">
					{sensors.ram_largest_free_block.toFixed(1)} KiB
				</span>
			</div>
		</div>

		<div class="divider my-0"></div>

		<!-- Storage -->

		<div class="flex items-center gap-3">
			<div class="rounded-xl bg-success/10 p-3 text-success">
				<HardDrive class="size-6" />
			</div>

			<div>
				<h2 class="card-title">Storage</h2>

				<p class="text-sm text-base-content/60">Flash and filesystem usage.</p>
			</div>
		</div>

		<div class="space-y-6 rounded-xl border border-base-300 bg-base-200/40 p-5">
			<div>
				<div class="mb-2 flex justify-between">
					<span>Flash</span>

					<span class="font-mono">
						{sensors.flash_used.toFixed(2)} /
						{sensors.flash_total.toFixed(2)}
						MiB
					</span>
				</div>

				<progress class="progress w-full progress-warning" value={flashPercent} max="100"
				></progress>
			</div>

			<div>
				<div class="mb-2 flex justify-between">
					<span>Code Storage</span>

					<span class="font-mono">
						{sensors.code_storage_used.toFixed(0)} /
						{sensors.code_storage_total.toFixed(0)}
						KiB
					</span>
				</div>

				<progress class="progress w-full progress-info" value={codePercent} max="100"></progress>
			</div>

			<div>
				<div class="mb-2 flex justify-between">
					<span>Website Storage</span>

					<span class="font-mono">
						{sensors.web_storage_used.toFixed(0)} /
						{sensors.web_storage_total.toFixed(0)}
						KiB
					</span>
				</div>

				<progress class="progress w-full progress-info" value={webPercent} max="100"></progress>
			</div>

			<div>
				<div class="mb-2 flex justify-between">
					<span>Configuration Storage</span>

					<span class="font-mono">
						{sensors.config_storage_used.toFixed(0)} /
						{sensors.config_storage_total.toFixed(0)}
						KiB
					</span>
				</div>

				<progress class="progress w-full progress-success" value={configPercent} max="100"
				></progress>
			</div>
		</div>
	</div>
</div>
