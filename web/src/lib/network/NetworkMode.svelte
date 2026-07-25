<script lang="ts">
	import DataInput from '$lib/devices/Cards/DataInput.svelte';
	import { Lock } from '@lucide/svelte';
	import { type NetworkConfig, WifiMode } from './NetworkTypes';

	let { networkConfig = $bindable() }: { networkConfig: NetworkConfig } = $props();
</script>

<fieldset class="fieldset bg-base-100 border border-base-300 rounded-box p-4">
	<legend class="fieldset-legend">Network Mode</legend>

	<div class="flex flex-col gap-3">
		<label
			class="label cursor-pointer justify-start gap-4 rounded-box border border-base-300 p-4 transition-colors has-checked:border-primary has-checked:bg-primary/10"
		>
			<input
				type="radio"
				name="network-mode"
				class="radio radio-primary"
				value={WifiMode.AP_STA}
				bind:group={networkConfig.mode}
			/>

			<div>
				<div class="font-medium">Access Point + Wi-Fi Client</div>
				<div class="text-base-content/70 text-sm text-wrap">
					Connect this device to your Wi-Fi network while allowing direct connections to its own
					Wi-Fi network for configuration.
				</div>
			</div>
		</label>

		<label
			class="label cursor-pointer justify-start gap-4 rounded-box border border-base-300 p-4 transition-colors has-checked:border-primary has-checked:bg-primary/10"
		>
			<input
				type="radio"
				name="network-mode"
				class="radio radio-primary"
				value={WifiMode.STA}
				bind:group={networkConfig.mode}
			/>

			<div>
				<div class="font-medium">Wi-Fi Client</div>
				<div class="text-base-content/70 text-sm text-wrap">
					Connect this device to your Wi-Fi network using your existing router. Direct
					connections to the device's Wi-Fi network will be unavailable.
				</div>
			</div>
		</label>
	</div>
</fieldset>
{#if networkConfig.mode === WifiMode.AP_STA}
<fieldset class="fieldset bg-base-100 border border-base-300 rounded-box p-4 mt-4">
			

	<legend class="fieldset-legend">Access Point Security</legend>

	<DataInput
				label="Access Point SSID"
				placeholder="Enter AP SSID"
				required
				type="text"
				icon={Lock}
				bind:value={networkConfig.apSsid}
			/>

	<label class="label cursor-pointer justify-start gap-3 mt-3">
		<input
			type="checkbox"
			class="checkbox checkbox-primary"
			bind:checked={networkConfig.apPasswordEnabled}
		/>

		<div>
			<div class="font-medium">Require Password</div>
			<div class="text-base-content/70 text-sm">
				If disabled, anyone nearby can connect to the device's access point.
			</div>
		</div>
	</label>

	{#if networkConfig.apPasswordEnabled}
		<div class="mt-3">
			<DataInput
				label="Access Point Password"
				placeholder="Enter AP password"
				required
				type="password"
				icon={Lock}
				bind:value={networkConfig.apPassword}
			/>
		</div>
	{/if}
</fieldset>
{/if}