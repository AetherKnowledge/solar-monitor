<script lang="ts">
	import DataInput from '$lib/devices/Cards/DataInput.svelte';
	import { Lock } from '@lucide/svelte';
	import { type NetworkConfig, WifiMode } from './NetworkTypes';

	let { networkConfig = $bindable() }: { networkConfig: NetworkConfig } = $props();
</script>

<fieldset class="fieldset rounded-box border border-base-300 bg-base-100 p-4">
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
				<div class="text-sm text-wrap text-base-content/70">
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
				<div class="text-sm text-wrap text-base-content/70">
					Connect this device to your Wi-Fi network using your existing router. Direct connections
					to the device's Wi-Fi network will be unavailable.
				</div>
			</div>
		</label>
	</div>
</fieldset>
{#if networkConfig.mode === WifiMode.AP_STA}
	<fieldset class="mt-4 fieldset rounded-box border border-base-300 bg-base-100 p-4">
		<legend class="fieldset-legend">Access Point Security</legend>

		<DataInput
			label="Access Point SSID"
			placeholder="Enter AP SSID"
			required
			type="text"
			icon={Lock}
			bind:value={networkConfig.apSsid}
		/>

		<label class="label mt-3 cursor-pointer justify-start gap-3">
			<input
				type="checkbox"
				class="checkbox checkbox-primary"
				bind:checked={networkConfig.apPasswordEnabled}
			/>

			<div>
				<div class="font-medium">Require Password</div>
				<div class="text-sm text-base-content/70">
					If disabled, anyone nearby can connect to the device's access point.
				</div>
			</div>
		</label>

		{#if networkConfig.apPasswordEnabled}
			<div class="mt-3">
				<DataInput
					label="Access Point Password"
					placeholder={networkConfig.hasApPassword ? 'Password is configured' : 'Enter AP password'}
					required={networkConfig.hasApPassword ? false : true}
					type="password"
					icon={Lock}
					bind:value={networkConfig.apPassword}
				/>
				{#if networkConfig.hasApPassword && networkConfig.apPassword === ''}
					<span class="mt-1 text-xs text-base-content/60">
						A password is already configured. Leave this field empty to keep the current password.
					</span>
				{/if}
			</div>
		{/if}
	</fieldset>
{/if}
