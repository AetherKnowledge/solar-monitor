<script lang="ts">
	import type { Snippet } from 'svelte';

	const { children }: { children: Snippet } = $props();
</script>

<main class="flex h-full flex-1 flex-col overflow-auto p-4">
	<div class="flex min-w-0 flex-1 flex-col">
		{@render children()}
	</div>
</main>
