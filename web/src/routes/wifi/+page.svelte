<script lang="ts">
	import NetworkMode from '$lib/components/Wifi/NetworkMode.svelte';
	import WifiNetwork from '$lib/components/Wifi/WifiNetwork.svelte';
	import type { NetworkConfig } from '$lib/types/NetworkData';

	let savedNetworkConfig = $state<NetworkConfig>({
		networkMode: 'ap+sta',
		ssid: 'Home Wi-Fi',
		password: ''
	});

	// svelte-ignore state_referenced_locally
	let networkConfig: NetworkConfig = $state($state.snapshot(savedNetworkConfig));

	let form: HTMLFormElement;
	let isValid = $state(false);

	function updateValidity() {
		isValid = form.checkValidity();
	}

	$effect(() => {
		// updates the validity of the form whenever the networkConfig changes

		// eslint-disable-next-line @typescript-eslint/no-unused-expressions
		networkConfig.ssid;

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
	<WifiNetwork bind:networkConfig />
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
