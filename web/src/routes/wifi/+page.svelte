<script lang="ts">
	import NetworkMode from '$lib/components/Wifi/NetworkMode.svelte';
	import WifiNetwork from '$lib/components/Wifi/WifiNetwork.svelte';
	import type { NetworkConfig } from '$lib/types/NetworkData';

	let savedNetworkConfig = $state<NetworkConfig>({
		networkMode: 'wifi',
		network: {
			ssid: 'Home Wi-Fi',
			signal: 5,
			secure: true,
			saved: true,
			connected: true
		},
		password: ''
	});

	// svelte-ignore state_referenced_locally
	let networkConfig = $state($state.snapshot(savedNetworkConfig));

	let form: HTMLFormElement;
	let isValid = $state(false);

	function updateValidity() {
		isValid = form.checkValidity();
	}

	$effect(() => {
		networkConfig.networkMode;
		networkConfig.network?.secure;

		updateValidity();
	});

	let hasChanged = $derived.by(
		() => JSON.stringify(networkConfig) !== JSON.stringify(savedNetworkConfig)
	);

	function cancelChanges() {
		networkConfig = $state.snapshot(savedNetworkConfig);
	}

	function saveChanges() {
		savedNetworkConfig = $state.snapshot(networkConfig);

		console.log('Saving network configuration:', networkConfig);
	}
</script>

<form bind:this={form} oninput={updateValidity} onchange={updateValidity}>
	<NetworkMode bind:networkConfig />
	{#if networkConfig.networkMode === 'wifi'}
		<WifiNetwork bind:networkConfig />
	{/if}
	<div class="flex justify-between">
		<button
			type="button"
			class="btn btn-error mt-4 w-20"
			disabled={!hasChanged}
			onclick={cancelChanges}
		>
			Cancel
		</button>
		<button
			type="submit"
			class="btn btn-success mt-4 w-20"
			disabled={!hasChanged || !isValid}
			onclick={saveChanges}
		>
			Save
		</button>
	</div>
</form>
