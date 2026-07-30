<script lang="ts">
	import type { Snippet } from 'svelte';

	const { children }: { children: Snippet } = $props();
</script>

<main class="flex min-h-0 flex-1 p-4">
	<div class="flex min-h-0 min-w-0 flex-1 flex-col">
		{@render children()}
	</div>
</main>
