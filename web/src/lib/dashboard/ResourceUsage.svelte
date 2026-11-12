<script lang="ts">
	type Props = {
		label: string;
		usage: number;
		total: number;
		unit: string;
		progressClass?:
			| 'progress-primary'
			| 'progress-info'
			| 'progress-success'
			| 'progress-warning'
			| 'progress-error';
		badge?: boolean;
		compact?: boolean;
	};

	const {
		label,
		usage,
		total,
		badge = false,
		unit,
		progressClass = 'progress-primary',
		compact = false
	}: Props = $props();

	let percent = $derived(() => {
		if (total <= 0) return 0;

		const value = (usage / total) * 100;
		return Number.isFinite(value) ? value : 0;
	});

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

	const health = $derived(getHealth(percent()));
</script>

{#if compact}
	<div>
		<div class="mb-2 flex justify-between">
			<span>{label}</span>

			<span class="font-mono">
				{usage.toFixed(2)} /
				{total.toFixed(2)}
				{unit}
			</span>
		</div>

		<progress class={`progress w-full ${progressClass}`} value={percent()} max="100"></progress>
	</div>
{:else}
	<div>
		<div class="mb-2 flex items-center justify-between">
			<div>
				<div class="font-medium">{label}</div>

				<div class="text-sm text-base-content/60">
					{usage.toFixed(1)}
					/
					{total.toFixed(1)}
					{unit}
				</div>
			</div>

			{#if badge}
				<div class={`badge gap-1 ${health.class}`}>
					<span class="size-2 rounded-full bg-current"></span>
					{health.text}
				</div>
			{/if}
		</div>

		<progress class={`progress w-full ${progressClass}`} value={percent()} max="100"></progress>

		<div class="mt-2 flex justify-end text-sm text-base-content/60">
			{percent().toFixed(0)}%
		</div>
	</div>
{/if}
