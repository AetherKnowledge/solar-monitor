<script lang="ts">
	import { Calculator, Edit, Gauge, SlidersHorizontal } from '@lucide/svelte';

	import { RegisterType, type RegisterListItem } from '../DeviceTypes';

	type Props = {
		register: RegisterListItem;
		onEdit?: (register: RegisterListItem) => void;
	};

	let { register, onEdit }: Props = $props();

	const badgeClass = $derived.by(() => {
		switch (register.type) {
			case RegisterType.Read:
				return 'badge-primary';
			case RegisterType.Virtual:
				return 'badge-secondary';
			case RegisterType.Select:
				return 'badge-accent';
			case RegisterType.Number:
				return 'badge-info';
		}
	});

	const Icon = $derived.by(() => {
		switch (register.type) {
			case RegisterType.Read:
				return Gauge;
			case RegisterType.Virtual:
				return Calculator;
			default:
				return SlidersHorizontal;
		}
	});
</script>

<div class="card border border-base-300 bg-base-200">
	<div class="card-body gap-3 p-4">
		<div class="flex items-start justify-between gap-3">
			<div class="min-w-0">
				<h3 class="truncate font-semibold">
					{register.name}
				</h3>

				<p class="text-xs text-base-content/60">
					{#if register.address !== undefined}
						Address {register.address}
					{:else}
						Virtual Register
					{/if}
				</p>
			</div>

			<div class={`badge ${badgeClass} gap-1`}>
				<Icon class="size-3" />
				{register.type}
			</div>
		</div>

		<div class="grid grid-cols-2 gap-y-2 text-sm">
			<div class="text-base-content/60">Value</div>
			<div class="text-right font-medium">
				{register.value}
			</div>

			<div class="text-base-content/60">Unit</div>
			<div class="text-right">
				{register.unit ?? '-'}
			</div>
		</div>

		<button class="btn mt-2 btn-primary btn-sm" onclick={() => onEdit?.(register)}>
			<Edit class="size-4" />
			Edit Register
		</button>
	</div>
</div>
