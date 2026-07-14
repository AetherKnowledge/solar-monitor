<script lang="ts">
	import NetworkMode from '$lib/components/Wifi/NetworkMode.svelte';
	import WifiNetwork from '$lib/components/Wifi/WifiNetwork.svelte';

	let networkMode: 'ap' | 'wifi' = $state('wifi');
</script>

<main class="flex flex-col p-4">
	<NetworkMode bind:networkMode />
	{#if networkMode === 'wifi'}
		<WifiNetwork />
	{/if}
</main>
