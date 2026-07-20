<script lang="ts">
	import { List, Plus, Trash2 } from '@lucide/svelte';

	type Props = {
		label: string;
		description?: string;
		placeholder?: string;
		options: string[];
	};

	let {
		label,
		description = '',
		placeholder = 'New option',
		options = $bindable()
	}: Props = $props();

	function addOption() {
		options = [...options, ''];
	}

	function removeOption(index: number) {
		options = options.filter((_, i) => i !== index);
	}
</script>

<div class="form-control w-full">
	<div class="label">
		<span class="label-text flex items-center gap-1 text-sm">
			<List class="size-4 text-secondary" />
			{label}
		</span>
	</div>

	<div class="rounded-xl border border-base-300 bg-base-100">
		{#if options.length === 0}
			<div class="py-8 text-center text-sm text-base-content/50">No options added.</div>
		{:else}
			<div class="divide-y divide-base-300">
				{#each options, index (index)}
					<div class="flex items-center gap-3 p-3">
						<input
							class="input input-sm input-bordered flex-1"
							{placeholder}
							bind:value={options[index]}
						/>

						<button
							class="btn btn-sm btn-error btn-soft btn-square"
							onclick={() => removeOption(index)}
						>
							<Trash2 class="size-4" />
						</button>
					</div>
				{/each}
			</div>
		{/if}

		<div class="border-t border-base-300 p-3">
			<button class="btn btn-sm btn-outline btn-primary w-full" onclick={addOption}>
				<Plus class="size-4" />
				Add Option
			</button>
		</div>
	</div>

	{#if description}
		<div class="label">
			<span class="label-text-alt text-xs">
				{description}
			</span>
		</div>
	{/if}
</div>
