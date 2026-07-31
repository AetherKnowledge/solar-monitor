<script lang="ts">
	import favicon from '$lib/assets/favicon.svg';
	import PopupHost from '$lib/popup/PopupHost.svelte';
	import './layout.css';

	let { children } = $props();
</script>

<svelte:head>
	<link rel="icon" href={favicon} />
	<link rel="preload" href="/roboto.woff2" as="font" type="font/woff2" crossorigin="anonymous" />
</svelte:head>

{@render children()}
<PopupHost />
