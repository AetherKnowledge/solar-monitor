<script lang="ts">
	import type { Component } from 'svelte';

	type Props = {
		label: string;
		description?: string;
		placeholder?: string;
		icon?: Component;
		iconColor?: string;
		options?: string[];

		value?: string | number;
	};

	let {
		label,
		description = '',
		placeholder = '',
		options = [],
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

	<select class="select select-bordered w-full" {placeholder} bind:value>
		{#each options as option (option)}
			<option value={option}>{option}</option>
		{/each}
	</select>

	{#if description}
		<div class="label">
			<span class="text-xs">
				{description}
			</span>
		</div>
	{/if}
</label>
