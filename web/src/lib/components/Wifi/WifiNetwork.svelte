<script lang="ts">
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

	type Network = {
		ssid: string;
		signal: 1 | 2 | 3 | 4 | 5;
		secure: boolean;
		saved?: boolean;
		connected?: boolean;
	};

	let networks = $state<Network[]>([
		{
			ssid: 'Home Wi-Fi',
			signal: 5,
			secure: true,
			saved: true,
			connected: true
		},
		{
			ssid: 'Office',
			signal: 4,
			secure: true
		},
		{
			ssid: 'PLDT_Fiber_2.4G',
			signal: 4,
			secure: true,
			saved: true
		},
		{
			ssid: 'Coffee Shop',
			signal: 3,
			secure: false
		},
		{
			ssid: 'GlobeAtHome',
			signal: 2,
			secure: true
		},
		{
			ssid: 'Guest',
			signal: 1,
			secure: false
		}
	]);

	let password = $state('');
	let showPassword = $state(false);

	let open = $state(false);
	let popover: HTMLUListElement;
	let searchInput = $state('');

	const filtered = $derived(
		networks
			.filter((n) => n.ssid.toLowerCase().includes(searchInput.toLowerCase()))
			.sort((a, b) => b.signal - a.signal)
	);

	// svelte-ignore state_referenced_locally
	let selected = $state(networks[0]);

	function select(network: Network) {
		selected = network;
		searchInput = '';

		popover.hidePopover();
	}

	function signalIcon(level: number) {
		switch (level) {
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

	let scanning = $state(false);
	async function refreshNetworks() {
		scanning = true;

		await new Promise((r) => setTimeout(r, 1500));

		networks = [...networks].sort(() => Math.random() - 0.5);

		scanning = false;
	}
</script>

<fieldset class="fieldset bg-base-100 border border-base-300 rounded-box p-4">
	<legend class="fieldset-legend">Wifi Networks</legend>

	<span class="text-base-content/50 font-medium"> Wi-Fi Network </span>
	<div class="join pb-2" style="anchor-name:--anchor-1">
		<label class="input w-full join-item">
			<Wifi class="size-4 opacity-60" />
			<input bind:value={selected.ssid} type="search" class="grow" readonly placeholder="Search" />
			<div class="flex items-center gap-2">
				{@render SignalIcon(selected.signal)}
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
						<input bind:value={searchInput} type="search" required placeholder="Search" />
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
			{#if scanning}
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

	{#if selected.secure}
		<span class="text-base-content/50 font-medium"> Wi-Fi Password </span>
		<div class="join">
			<label class="input w-full join-item">
				<KeyRound class="size-4 opacity-60" />

				<input
					class="grow"
					type={showPassword ? 'text' : 'password'}
					placeholder="Enter Wi-Fi password"
					bind:value={password}
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
	{/if}
</fieldset>

{#snippet NetworkBar(network: Network)}
	<li>
		<button type="button" class="justify-between" onclick={() => select(network)}>
			<div class="flex items-center gap-3">
				{#if network.secure}
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
				{@render SignalIcon(network.signal)}

				{#if selected.ssid === network.ssid}
					<Check class="size-4 text-primary" />
				{/if}
			</div>
		</button>
	</li>
{/snippet}

{#snippet SignalIcon(level: number)}
	{@const Icon = signalIcon(level)}

	<Icon class="size-4 opacity-70 text-accent" />
{/snippet}
