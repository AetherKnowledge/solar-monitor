<script lang="ts">
	import { UpdateStatus } from '$lib/common/CommonTypes';
	import NetworkMode from '$lib/network/NetworkMode.svelte';
	import { createNetworkConfigQuery, updateNetworkConfig } from '$lib/network/NetworkQueries';
	import { defaultNetworkConfig, type NetworkConfig } from '$lib/network/NetworkTypes';
	import WifiNetwork from '$lib/network/WifiNetworks.svelte';
	import { hidePopup, showError, showLoading } from '$lib/popup/Popup.svelte';

	let query = createNetworkConfigQuery();
	let savedNetworkConfig = $derived(query.data);

	let networkConfig = $state<NetworkConfig>(defaultNetworkConfig);

	let initialized = $state(false);
	$effect(() => {
		if (!initialized && savedNetworkConfig) {
			networkConfig = { ...savedNetworkConfig };
			initialized = true;
		}
	});

	$effect(() => {
		if (query.isPending) {
			showLoading('Loading network configuration...');
		} else if (
			query.data?.updateStatus === UpdateStatus.InProgress ||
			query.data?.updateStatus === UpdateStatus.Requested
		) {
			showLoading('Updating network configuration...');
		} else {
			hidePopup();
		}
	});

	let form: HTMLFormElement | undefined = $state(undefined);
	let isValid = $state(false);

	function updateValidity() {
		isValid = form?.checkValidity() ?? false;
	}

	$effect(() => {
		// updates the validity of the form whenever the networkConfig changes
		// eslint-disable-next-line @typescript-eslint/no-unused-expressions
		networkConfig?.ssid;

		updateValidity();
	});

	let hasChanged = $derived.by(
		() => JSON.stringify(networkConfig) !== JSON.stringify(savedNetworkConfig)
	);

	function cancelChanges() {
		if (savedNetworkConfig) {
			networkConfig = { ...savedNetworkConfig };
		}
	}

	async function saveChanges() {
		try {
			await updateNetworkConfig(networkConfig);
			await query.refetch();
		} catch {
			showError('Failed to save network configuration. Please try again.');
			return;
		}
	}
</script>

<form
	bind:this={form}
	oninput={updateValidity}
	onchange={updateValidity}
	onsubmit={(e) => {
		e.preventDefault();
		saveChanges();
	}}
>
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
		<button type="submit" class="btn btn-success mt-4 w-20" disabled={!hasChanged || !isValid}>
			Save
		</button>
	</div>
</form>
