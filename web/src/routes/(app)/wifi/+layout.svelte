<script lang="ts">
	import ScrollPage from '$lib/common/ScrollPage.svelte';
	import type { Snippet } from 'svelte';

	const { children }: { children: Snippet } = $props();
</script>

<ScrollPage>
	{@render children()}
</ScrollPage>
