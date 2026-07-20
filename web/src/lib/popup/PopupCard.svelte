<script lang="ts">
	import type { Snippet } from 'svelte';

	type Props = {
		children?: Snippet;
	};

	let { children }: Props = $props();
</script>

<div
	class="card bg-base-100/80 backdrop-blur-xl shadow-2xl border border-base-300 w-96 animate-fade-up"
>
	{#if children}
		{@render children()}
	{/if}
</div>
