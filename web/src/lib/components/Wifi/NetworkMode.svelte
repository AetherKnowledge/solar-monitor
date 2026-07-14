<script lang="ts">
	import type { NetworkConfig } from '$lib/types/NetworkData';

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
				value="ap+sta"
				bind:group={networkConfig.networkMode}
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
				value="sta"
				bind:group={networkConfig.networkMode}
			/>

			<div>
				<div class="font-medium">Wi-Fi Client</div>
				<div class="text-base-content/70 text-sm text-wrap">
					Connect this device to your Wi-Fi network using your existing router. Direct connections
					to the device's Wi-Fi network will be unavailable.
				</div>
			</div>
		</label>
	</div>
</fieldset>
