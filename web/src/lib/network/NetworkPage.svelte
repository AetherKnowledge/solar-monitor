<script lang="ts">
	import { type NetworkConfig } from '$lib/config/ConfigTypes';
	import { createNetworkQueryOptions, scanNetworks } from '$lib/network/NetworkQueries';
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
	import { createQuery } from '@tanstack/svelte-query';
	import { onMount } from 'svelte';
	import { type WifiNetwork, WifiScanStatus } from './NetworkTypes';

	type Props = {
		networkConfig: NetworkConfig;
	};

	let { networkConfig: selected = $bindable() }: Props = $props();

	const networksQuery = createQuery(() => createNetworkQueryOptions());
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
		selected.ssid = network.ssid;
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

	function findNetwork(ssid: string = '') {
		return networks.find((n) => n.ssid === ssid);
	}
</script>

<fieldset class="fieldset bg-base-100 border border-base-300 rounded-box p-4 gap-5">
	<legend class="fieldset-legend">Wifi Networks</legend>

	<div class="flex flex-col">
		<div class="flex flex-col">
			<span class="font-medium">Wi-Fi Network</span>
			<span class="text-base-content/50 text-sm">
				Select the wireless network the device will connect to.
			</span>
		</div>
		<div class="join pb-2" style="anchor-name:--anchor-1">
			<label class="input w-full join-item">
				<Wifi class="size-4 opacity-60" />
				<input
					value={findNetwork(selected.ssid)?.ssid || ''}
					type="search"
					class="grow"
					readonly
					required
					placeholder="Search"
				/>
				<div class="flex items-center gap-2">
					{@render SignalIcon(findNetwork(selected.ssid)?.rssi || 0)}
				</div>
			</label>
			<button
				class="btn btn-outline border-base-content/20 join-item w-15"
				type="button"
				popovertarget="popover-1"
			>
				<ChevronDown />
			</button>
			<ul
				class="dropdown dropdown-end menu w-200 rounded-box bg-base-100 shadow-sm p-2"
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
					<div class="join gap-0 p-0 bg-base-100">
						<label class="input w-full join-item">
							<Search class="size-4 opacity-60" />
							<input bind:value={searchInput} type="search" placeholder="Search" />
						</label>

						<button
							type="button"
							class="btn btn-outline border-base-content/20 join-item w-15"
							onclick={refreshNetworks}
						>
							<RefreshCcw class="size-4" />
						</button>
					</div>
				</li>
				{#if status === WifiScanStatus.SCAN_STATUS_IN_PROGRESS}
					<li class="menu-disabled flex flex-col items-center gap-2 py-4">
						<span class="loading loading-spinner loading-xl text-base-content">wew</span>
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

	{#if findNetwork(selected.ssid)?.encryptionType === 0}
		<div class="flex flex-col">
			<div class="flex flex-col">
				<span class="font-medium">Wi-Fi Password</span>
				<span class="text-base-content/50 text-sm">
					Enter the password for the selected Wi-Fi network.
				</span>
			</div>
			<div class="join">
				<label class="input w-full join-item">
					<KeyRound class="size-4 opacity-60" />

					<input
						class="grow"
						type={showPassword ? 'text' : 'password'}
						placeholder="Enter Wi-Fi password"
						required
						bind:value={selected.password}
					/>
				</label>

				<button
					type="button"
					class="btn btn-outline border-base-content/20 join-item w-15"
					onclick={() => (showPassword = !showPassword)}
				>
					{#if showPassword}
						<EyeOff class="size-4" />
					{:else}
						<Eye class="size-4" />
					{/if}
				</button>
			</div>
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

					<div class="flex gap-1 mt-1">
						{#if network.connected}
							<div class="badge badge-success badge-xs">Connected</div>
						{/if}

						{#if network.saved}
							<div class="badge badge-info badge-xs">Saved</div>
						{/if}
					</div>
				</div>
			</div>

			<div class="flex items-center gap-2">
				{@render SignalIcon(network.rssi)}

				{#if selected.ssid === network.ssid}
					<Check class="size-4 text-primary" />
				{/if}
			</div>
		</button>
	</li>
{/snippet}

{#snippet SignalIcon(rssi: number)}
	{@const Icon = signalIcon(rssi)}

	<Icon class="size-4 opacity-70 text-green-500" />
{/snippet}
