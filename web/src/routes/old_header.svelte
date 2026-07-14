<script lang="ts">
	import type { Snippet } from 'svelte';

	type Props = {
		name: string;
		children?: Snippet;
	};

	let { name, children }: Props = $props();
</script>

<h1>Hello, {name}!</h1>
{#if children}
	{@render children()}
{/if}
