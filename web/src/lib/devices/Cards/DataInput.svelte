<script lang="ts">
	import type { Component } from 'svelte';

	type Props = {
		label: string;
		description?: string;
		placeholder?: string;
		type?: string;
		icon?: Component;
		iconColor?: string;

		value?: string | number;
	};

	let {
		label,
		description = '',
		placeholder = '',
		type = 'text',
		icon: Icon,
		iconColor = 'text-primary',
		value = $bindable()
	}: Props = $props();
</script>

<label class="form-control">
	<div class="label">
		<span class="text-sm flex items-center gap-1">
			{#if Icon}
				<Icon class={iconColor + ' size-4'} />
			{/if}

			<p class="pt-0.5">
				{label}
			</p>
		</span>
	</div>

	<input class="input input-bordered w-full" {type} {placeholder} bind:value />

	{#if description}
		<div class="label">
			<span class="text-xs">
				{description}
			</span>
		</div>
	{/if}
</label>
