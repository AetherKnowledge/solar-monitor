<script lang="ts">
	import { HardDrive, MemoryStick, MonitorSmartphone, Thermometer, Timer } from '@lucide/svelte';

	import { createDashboardController } from './DashboardController.svelte';
	import ResourceUsage from './ResourceUsage.svelte';

	const { query } = createDashboardController();

	const sensors = $derived(query.data.sensors);

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
			<ResourceUsage
				label="RAM"
				usage={sensors.ram_usage}
				total={sensors.ram_total}
				unit=" KiB"
				badge
			/>

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
			<ResourceUsage
				label="Flash"
				usage={sensors.flash_used}
				total={sensors.flash_total}
				unit=" MiB"
				progressClass="progress-warning"
				compact
			/>

			<ResourceUsage
				label="Code"
				usage={sensors.code_storage_used}
				total={sensors.code_storage_total}
				unit=" KiB"
				compact
				progressClass="progress-info"
			/>

			<ResourceUsage
				label="Website"
				usage={sensors.web_storage_used}
				total={sensors.web_storage_total}
				unit=" KiB"
				compact
				progressClass="progress-info"
			/>

			<ResourceUsage
				label="Configuration"
				usage={sensors.config_storage_used}
				total={sensors.config_storage_total}
				unit=" KiB"
				compact
				progressClass="progress-success"
			/>
		</div>
	</div>
</div>
