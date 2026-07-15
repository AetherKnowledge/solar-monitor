<script lang="ts">
	import PageLoading from '$lib/loading/PageLoading.svelte';
	import NetworkMode from '$lib/network/NetworkMode.svelte';
	import WifiNetwork from '$lib/network/NetworkPage.svelte';
	import {
		createNetworkConfigQueryOptions,
		updateNetworkConfig
	} from '$lib/network/NetworkQueries';
	import { defaultNetworkConfig, type NetworkConfig } from '$lib/network/NetworkTypes';
	import { createQuery } from '@tanstack/svelte-query';

	let networksConfigQuery = createQuery(() => createNetworkConfigQueryOptions());
	let savedNetworkConfig = $derived(networksConfigQuery.data);
	let isPending = $derived(networksConfigQuery.isPending);
	let isSaving = $state(false);

	let networkConfig: NetworkConfig = $state(defaultNetworkConfig);

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

	let initialized = $state(false);
	$effect(() => {
		if (!initialized && savedNetworkConfig) {
			Object.assign(networkConfig, savedNetworkConfig);
			initialized = true;
		}
	});

	let hasChanged = $derived.by(
		() => JSON.stringify(networkConfig) !== JSON.stringify(savedNetworkConfig)
	);

	function cancelChanges() {
		if (savedNetworkConfig) {
			Object.assign(networkConfig, savedNetworkConfig);
		}
	}

	async function saveChanges() {
		isSaving = true;
		try {
			await updateNetworkConfig(networkConfig);
			await networksConfigQuery.refetch();
		} finally {
			isSaving = false;
		}
	}
</script>

<div class="relative h-full">
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

	{#if isPending}
		<PageLoading text="Loading network configuration..." />
	{:else if isSaving}
		<PageLoading text="Saving network configuration..." />
	{/if}
</div>
