<script lang="ts">
	import PageLoading from '$lib/loading/PageLoading.svelte';
	import MqttFillIcon from '@iconify-svelte/mingcute/mqtt-fill';
	import { EthernetPort, Eye, EyeOff, KeyRound, SearchCode, User } from '@lucide/svelte';
	import { createQuery } from '@tanstack/svelte-query';
	import { onMount } from 'svelte';
	import { slide } from 'svelte/transition';
	import { createMqttConfigQueryOptions, updateMqttConfig } from './MqttQueries';
	import { defaultMqttConfig, type MqttConfig } from './MqttTypes';

	let mqttConfigQuery = createQuery<MqttConfig>(() => createMqttConfigQueryOptions());
	let savedMqttConfig = $derived(mqttConfigQuery.data);
	let isPending = $derived(mqttConfigQuery.isPending);
	let isSaving = $state(false);

	let mqttConfig = $state(defaultMqttConfig);
	let initialized = $state(false);
	$effect(() => {
		if (!initialized && savedMqttConfig) {
			mqttConfig = { ...savedMqttConfig };
			initialized = true;
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

	async function saveChanges() {
		isSaving = true;
		try {
			await updateMqttConfig(mqttConfig);
			await mqttConfigQuery.refetch();
		} finally {
			isSaving = false;
		}

		console.log('Saving MQTT configuration:', mqttConfig);
	}

	$effect(() => {
		console.log('mqttConfig changed:', { ...mqttConfig });
		console.log('savedMqttConfig:', savedMqttConfig);
		console.log('hasChanged:', hasChanged);
	});
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
		<fieldset class="fieldset bg-base-100 border border-base-300 rounded-box p-4 gap-5">
			<legend class="fieldset-legend">MQTT Configuration</legend>

			<div class="flex flex-col">
				<div class="flex flex-col">
					<span class="font-medium">Broker</span>
					<span class="text-base-content/50 text-sm">
						The hostname or IP address of your MQTT broker.
					</span>
				</div>
				<label class="input validator w-full">
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
					<span class="text-base-content/50 text-sm">
						The network port used to connect to the MQTT broker. The default is 1883.
					</span>
				</div>
				<label class="input validator w-full">
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
					<span class="text-base-content/50 text-sm">
						A unique identifier for this device when connecting to the MQTT broker. Leave blank to
						generate one automatically.
					</span>
				</div>
				<label class="input validator w-full">
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
					<span class="text-base-content/50 text-sm">
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
					<span class="text-base-content/50 text-sm">
						Optional password used to authenticate with the MQTT broker.
					</span>
				</div>
				<div class="join">
					<label class="input w-full join-item">
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

			<div class="flex items-center justify-between">
				<div class="flex flex-col">
					<span class="font-medium">Home Assistant Autodiscovery</span>
					<span class="text-base-content/50 text-sm">
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
						<span class="text-base-content/50 text-sm">
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
		</fieldset>
	</form>
	{#if isPending}
		<PageLoading text="Loading mqtt configuration..." />
	{:else if isSaving}
		<PageLoading text="Saving mqtt configuration..." />
	{/if}
</div>
