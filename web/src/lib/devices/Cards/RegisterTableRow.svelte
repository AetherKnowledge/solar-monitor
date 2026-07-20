<script lang="ts">
	import { BookOpen, Hash, List, Pencil, SquareFunction } from '@lucide/svelte';
	import { RegisterType, type RegisterListItem } from '../DeviceTypes';

	type Props = {
		register: RegisterListItem;
		onEdit: (r: RegisterListItem) => void;
	};

	const { register, onEdit }: Props = $props();

	const badgeClass = $derived.by(() => {
		switch (register.type) {
			case RegisterType.Read:
				return 'badge-primary badge-soft';

			case RegisterType.Virtual:
				return 'badge-secondary badge-soft';

			case RegisterType.Select:
				return 'badge-accent badge-soft';

			case RegisterType.Number:
				return 'badge-warning badge-soft';
		}
	});

	const Icon = $derived.by(() => {
		switch (register.type) {
			case RegisterType.Read:
				return BookOpen;

			case RegisterType.Virtual:
				return SquareFunction;

			case RegisterType.Select:
				return List;

			case RegisterType.Number:
				return Hash;
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
			<div class={`badge min-w-24 justify-center gap-1 ${badgeClass}`}>
				<Icon class="size-3.5" />
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
			<button class="btn btn-sm btn-primary btn-soft gap-2" onclick={() => onEdit(register)}>
				<Pencil class="size-4" />
				Edit
			</button>
		</div>
	</td>
</tr>
