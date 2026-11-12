<script lang="ts">
	import { UpdateStatus } from '$lib/common/CommonTypes';
	import {
		Check,
		ChevronDown,
		Eye,
		EyeOff,
		KeyRound,
		Lock,
		LockOpen,
		RefreshCcw,
		Search,
		Signal,
		SignalHigh,
		SignalLow,
		SignalMedium,
		SignalZero,
		Wifi
	} from '@lucide/svelte';
	import { onMount } from 'svelte';
	import { createNetworkScanQuery, scanNetworks } from './NetworkQueries';
	import { type NetworkConfig, type WifiNetwork } from './NetworkTypes';

	type Props = {
		networkConfig: NetworkConfig;
	};

	let { networkConfig = $bindable() }: Props = $props();

	const networksQuery = createNetworkScanQuery();
	const status = $derived(networksQuery.data.status);
	const networks = $derived(networksQuery.data.networks);

	let showPassword = $state(false);

	let open = $state(false);
	let popover: HTMLUListElement;
	let searchInput = $state('');

	const filtered = $derived(
		networks
			.filter((n) => n.ssid.toLowerCase().includes(searchInput.toLowerCase()))
			.sort((a, b) => b.rssi - a.rssi)
	);

	function select(network: WifiNetwork) {
		networkConfig.wifiSsid = network.ssid;
		searchInput = '';

		popover.hidePopover();
	}

	function signalLevel(rssi: number): 1 | 2 | 3 | 4 | 5 {
		if (rssi >= -50) return 5;
		if (rssi >= -60) return 4;
		if (rssi >= -70) return 3;
		if (rssi >= -80) return 2;
		return 1;
	}

	function signalIcon(rssi: number) {
		switch (signalLevel(rssi)) {
			case 5:
				return Signal;
			case 4:
				return SignalHigh;
			case 3:
				return SignalMedium;
			case 2:
				return SignalLow;
			default:
				return SignalZero;
		}
	}

	async function refreshNetworks() {
		await scanNetworks();
		networksQuery.refetch();
	}

	onMount(() => {
		refreshNetworks();
	});

	const selectedNetwork = $derived.by(() =>
		networks.find((n) => n.ssid === networkConfig.wifiSsid)
	);
</script>

<fieldset class="fieldset gap-5 rounded-box border border-base-300 bg-base-100 p-4">
	<legend class="fieldset-legend">Wifi Networks</legend>

	<div class="flex flex-col">
		<div class="flex flex-col">
			<span class="font-medium">Wi-Fi Network</span>
			<span class="text-sm text-base-content/50">
				Select the wireless network the device will connect to.
			</span>
		</div>
		<div class="join pb-2" style="anchor-name:--anchor-1">
			<label class="input join-item w-full">
				<Wifi class="size-4 opacity-60" />
				<input
					value={selectedNetwork?.ssid || networkConfig.wifiSsid || ''}
					type="search"
					class="grow"
					readonly
					required
					placeholder="Search"
				/>
				<div class="flex items-center gap-2">
					{#if selectedNetwork}
						{@render SignalIcon(selectedNetwork.rssi)}
					{/if}
				</div>
			</label>
			<button
				class="btn join-item w-15 border-base-content/20 btn-outline"
				type="button"
				popovertarget="popover-1"
			>
				<ChevronDown />
			</button>
			<ul
				class="menu dropdown dropdown-end w-200 rounded-box bg-base-100 p-2 shadow-sm"
				bind:this={popover}
				popover
				id="popover-1"
				style="
		position-anchor: --anchor-1;
		width: anchor-size(width);
	"
				class:dropdown-open={open}
			>
				<li class="w-full py-2">
					<div class="join gap-0 bg-base-100 p-0">
						<label class="input join-item w-full">
							<Search class="size-4 opacity-60" />
							<input bind:value={searchInput} type="search" placeholder="Search" />
						</label>

						<button
							type="button"
							class="btn join-item w-15 border-base-content/20 btn-outline"
							onclick={refreshNetworks}
						>
							<RefreshCcw class="size-4" />
						</button>
					</div>
				</li>
				{#if status === UpdateStatus.InProgress}
					<li class="menu-disabled flex flex-col items-center gap-2 py-4">
						<span class="loading loading-xl loading-spinner text-base-content">wew</span>
						<span>Loading</span>
					</li>
				{:else if filtered.length === 0}
					<li class="menu-disabled flex flex-col items-center gap-2 py-4">
						<span>No networks found</span>
					</li>
				{:else}
					{#each filtered as network (network.ssid)}
						{@render NetworkBar(network)}
					{/each}
				{/if}
			</ul>
		</div>
	</div>

	{#if selectedNetwork?.encryptionType !== 0}
		<div class="flex flex-col">
			<div class="flex flex-col">
				<span class="font-medium">Wi-Fi Password</span>
				<span class="text-sm text-base-content/50">
					Enter the password for the selected Wi-Fi network.
				</span>
			</div>
			<div class="join">
				<label class="input join-item w-full">
					<KeyRound class="size-4 opacity-60" />

					<input
						class="grow"
						type={showPassword ? 'text' : 'password'}
						placeholder={networkConfig.hasWifiPassword
							? 'Password is configured'
							: 'Enter Wi-Fi password'}
						required={networkConfig.hasWifiPassword ? false : true}
						bind:value={networkConfig.wifiPassword}
					/>
				</label>

				<button
					type="button"
					class="btn join-item w-15 border-base-content/20 btn-outline"
					onclick={() => (showPassword = !showPassword)}
				>
					{#if showPassword}
						<EyeOff class="size-4" />
					{:else}
						<Eye class="size-4" />
					{/if}
				</button>
			</div>
			{#if networkConfig.hasWifiPassword && networkConfig.wifiPassword === ''}
				<span class="mt-1 text-xs text-base-content/60">
					A password is already configured. Leave this field empty to keep the current password.
				</span>
			{/if}
		</div>
	{/if}
</fieldset>

{#snippet NetworkBar(network: WifiNetwork)}
	<li>
		<button type="button" class="justify-between" onclick={() => select(network)}>
			<div class="flex items-center gap-3">
				{#if network.encryptionType !== 0}
					<Lock class="size-4 opacity-60" />
				{:else}
					<LockOpen class="size-4 opacity-40" />
				{/if}

				<div class="text-left">
					<div class="font-medium">
						{network.ssid}
					</div>

					<div class="mt-1 flex gap-1">
						{#if networkConfig.wifiSsid === network.ssid}
							<div class="badge badge-xs badge-success">Connected</div>
						{/if}

						{#if network.saved}
							<div class="badge badge-xs badge-info">Saved</div>
						{/if}
					</div>
				</div>
			</div>

			<div class="flex items-center gap-2">
				{@render SignalIcon(network.rssi)}

				{#if networkConfig.wifiSsid === network.ssid}
					<Check class="size-4 text-primary" />
				{/if}
			</div>
		</button>
	</li>
{/snippet}

{#snippet SignalIcon(rssi: number)}
	{@const Icon = signalIcon(rssi)}

	<Icon class="size-4 text-green-500 opacity-70" />
{/snippet}
