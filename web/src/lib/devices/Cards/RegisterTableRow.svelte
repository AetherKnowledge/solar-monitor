<script lang="ts">
	import { Pencil } from '@lucide/svelte';
	import { RegisterType } from '../DeviceTypes';

	export interface RegisterListItem {
		id: string;
		type: RegisterType;

		name: string;
		address?: number;

		value?: string;
		unit?: string;
	}

	type Props = {
		register: RegisterListItem;
		onEdit: (id: string) => void;
	};

	const { register, onEdit }: Props = $props();

	const badgeClass = $derived.by(() => {
		switch (register.type) {
			case RegisterType.Read:
				return 'badge-primary';

			case RegisterType.Virtual:
				return 'badge-secondary';

			case RegisterType.Select:
				return 'badge-accent';

			case RegisterType.Number:
				return 'badge-warning';

			default:
				return 'badge-neutral';
		}
	});
</script>

<tr class="hover">
	<td>
		<div class="font-semibold">
			{register.name}
		</div>
	</td>

	<td>
		<div class="flex justify-center">
			<div class={`badge ${badgeClass} badge-soft w-20 justify-center`}>
				{register.type}
			</div>
		</div>
	</td>

	<td>
		<div class="text-center font-mono text-sm tabular-nums">
			{register.address ?? '—'}
		</div>
	</td>

	<td>
		<div class="text-center font-mono tabular-nums whitespace-nowrap">
			{register.value ?? '—'}
		</div>
	</td>

	<td>
		<div class="text-center text-base-content/70 whitespace-nowrap">
			{register.unit || '—'}
		</div>
	</td>

	<td>
		<div class="flex justify-end">
			<button class="btn btn-sm btn-primary btn-soft gap-2" onclick={() => onEdit(register.id)}>
				<Pencil class="size-4" />
				Edit
			</button>
		</div>
	</td>
</tr>
