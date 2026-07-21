<script lang="ts">
	import { hidePopup, showError, showLoading, showSuccess } from '$lib/popup/Popup.svelte';
	import MqttFillIcon from '@iconify-svelte/mingcute/mqtt-fill';
	import { EthernetPort, Eye, EyeOff, KeyRound, SearchCode, User } from '@lucide/svelte';
	import { onMount } from 'svelte';
	import { slide } from 'svelte/transition';
	import { createMqttConfigQuery, updateMqttConfig } from './MqttQueries';
	import { defaultMqttConfig } from './MqttTypes';

	let mqttConfigQuery = createMqttConfigQuery();
	let savedMqttConfig = $derived(mqttConfigQuery.data);
	let isPending = $derived(mqttConfigQuery.isPending);

	let mqttConfig = $state(defaultMqttConfig);
	let initialized = $state(false);
	$effect(() => {
		if (!initialized && savedMqttConfig) {
			mqttConfig = { ...savedMqttConfig };
			initialized = true;
		}
	});

	$effect(() => {
		if (isPending) {
			showLoading('Loading MQTT configuration...');
		} else {
			hidePopup();
		}
	});

	let showPassword = $state(false);

	let form: HTMLFormElement;
	let isValid = $state(false);

	function updateValidity() {
		isValid = form.checkValidity();
	}

	let hasChanged = $derived.by(
		() => JSON.stringify(mqttConfig) !== JSON.stringify(savedMqttConfig)
	);

	onMount(() => {
		isValid = form.checkValidity();
	});

	function cancelChanges() {
		if (savedMqttConfig) {
			mqttConfig = { ...savedMqttConfig };
		}
	}

	onMount(() => {
		return () => {
			hidePopup();
		};
	});

	async function saveChanges() {
		showLoading('Saving MQTT configuration...');
		try {
			await updateMqttConfig(mqttConfig);
			await mqttConfigQuery.refetch();
		} catch {
			showError('Failed to save MQTT configuration. Please try again.');
			return;
		}

		showSuccess('MQTT configuration saved successfully.');
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
	<div class="card border border-base-300 bg-base-100 shadow-sm">
		<div class="card-body gap-6">
			<div class="flex items-center gap-3">
				<div class="rounded-xl bg-primary/10 p-3 text-primary">
					<MqttFillIcon class="size-6" />
				</div>

				<div>
					<h2 class="card-title">MQTT Configuration</h2>
					<p class="text-sm text-base-content/60">
						Configure how this device connects to your MQTT broker.
					</p>
				</div>
			</div>

			<div class="flex flex-col">
				<div class="flex flex-col">
					<span class="font-medium">Broker</span>
					<span class="text-sm text-base-content/50">
						The hostname or IP address of your MQTT broker.
					</span>
				</div>
				<label class="validator input w-full">
					<MqttFillIcon class="size-4 opacity-60" />
					<input
						bind:value={mqttConfig.host}
						type="text"
						required
						class="grow"
						placeholder="Enter MQTT broker address"
					/>
				</label>
			</div>

			<div class="flex flex-col">
				<div class="flex flex-col">
					<span class="font-medium">Port</span>
					<span class="text-sm text-base-content/50">
						The network port used to connect to the MQTT broker. The default is 1883.
					</span>
				</div>
				<label class="validator input w-full">
					<EthernetPort class="size-4 opacity-60" />
					<input
						class="grow"
						type="number"
						min="0"
						max="65535"
						step="1"
						placeholder="Enter port"
						required
						bind:value={mqttConfig.port}
					/>
				</label>
			</div>

			<div class="flex flex-col">
				<div class="flex flex-col">
					<span class="font-medium">Client ID</span>
					<span class="text-sm text-base-content/50">
						A unique identifier for this device when connecting to the MQTT broker. Leave blank to
						generate one automatically.
					</span>
				</div>
				<label class="validator input w-full">
					<MqttFillIcon class="size-4 opacity-60" />
					<input
						bind:value={mqttConfig.clientId}
						type="text"
						class="grow"
						placeholder="Enter client ID"
					/>
				</label>
			</div>

			<div class="flex flex-col">
				<div class="flex flex-col">
					<span class="font-medium">Username</span>
					<span class="text-sm text-base-content/50">
						Optional username used to authenticate with the MQTT broker.
					</span>
				</div>
				<label class="input w-full">
					<User class="size-4 opacity-60" />
					<input
						class="grow"
						type="text"
						placeholder="Enter username"
						bind:value={mqttConfig.username}
					/>
				</label>
			</div>

			<div class="flex flex-col">
				<div class="flex flex-col">
					<span class="font-medium">Password</span>
					<span class="text-sm text-base-content/50">
						Optional password used to authenticate with the MQTT broker.
					</span>
				</div>
				<div class="join">
					<label class="input join-item w-full">
						<KeyRound class="size-4 opacity-60" />

						<input
							class="grow"
							type={showPassword ? 'text' : 'password'}
							placeholder="Enter password"
							bind:value={mqttConfig.password}
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
			</div>

			<div class="flex items-center justify-between">
				<div class="flex flex-col">
					<span class="font-medium">Home Assistant Autodiscovery</span>
					<span class="text-sm text-base-content/50">
						Automatically publish Home Assistant discovery topics.
					</span>
				</div>

				<input
					type="checkbox"
					class="toggle toggle-primary"
					bind:checked={mqttConfig.autoDiscoveryEnabled}
				/>
			</div>

			{#if mqttConfig.autoDiscoveryEnabled}
				<div transition:slide class="flex flex-col overflow-hidden">
					<div class="flex flex-col">
						<span class="font-medium">Autodiscovery Prefix</span>
						<span class="text-sm text-base-content/50">
							The MQTT topic prefix Home Assistant listens to for discovery messages. The default is <code
								>homeassistant</code
							>.
						</span>
					</div>

					<label class="input w-full">
						<SearchCode class="size-4 opacity-60" />
						<input
							class="grow"
							type="text"
							placeholder="Enter autodiscovery prefix"
							bind:value={mqttConfig.autoDiscoveryPrefix}
						/>
					</label>
				</div>
			{/if}
			<div class="flex justify-between">
				<button
					type="button"
					class="btn mt-4 w-20 btn-error"
					disabled={!hasChanged}
					onclick={cancelChanges}
				>
					Cancel
				</button>
				<button type="submit" class="btn mt-4 w-20 btn-success" disabled={!hasChanged || !isValid}>
					Save
				</button>
			</div>
		</div>
	</div>
</form>
