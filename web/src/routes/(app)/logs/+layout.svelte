<script lang="ts">
	import FullHeightPage from '$lib/common/FullHeightPage.svelte';
	import type { Snippet } from 'svelte';

	const { children }: { children: Snippet } = $props();
</script>

<FullHeightPage>
	{@render children()}
</FullHeightPage>
