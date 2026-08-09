<script lang="ts">
	import { createPatch } from '$lib/common/Patcher.svelte';
	import { hidePopup, showError, showLoading, showSuccess } from '$lib/popup/Popup.svelte';
	import { Eye, EyeOff, KeyRound, LockKeyhole } from '@lucide/svelte';
	import { onMount } from 'svelte';
	import { slide } from 'svelte/transition';

	import { createSiteConfigQuery, updateSiteConfig } from './AuthQueries';
	import { defaultSiteConfig } from './AuthTypes';

	let siteConfigQuery = createSiteConfigQuery();

	let savedSiteConfig = $derived(siteConfigQuery.data);
	let isPending = $derived(siteConfigQuery.isPending);

	let siteConfig = $state({ ...defaultSiteConfig });
	let initialized = $state(false);

	let showPassword = $state(false);

	let form: HTMLFormElement;
	let isValid = $state(false);

	$effect(() => {
		if (!initialized && savedSiteConfig) {
			siteConfig = { ...savedSiteConfig };
			initialized = true;
		}
	});

	$effect(() => {
		if (isPending) {
			showLoading('Loading authentication configuration...');
		} else {
			hidePopup();
		}
	});

	onMount(() => {
		isValid = form.checkValidity();
	});

	onMount(() => {
		return () => {
			hidePopup();
		};
	});

	function updateValidity() {
		isValid = form.checkValidity();
	}

	function cancelChanges() {
		if (savedSiteConfig) {
			siteConfig = { ...savedSiteConfig };
		}
	}

	const hasChanged = $derived.by(() => {
		if (!savedSiteConfig || !siteConfig) {
			return false;
		}

		return createPatch(savedSiteConfig, siteConfig) !== undefined;
	});

	async function saveChanges() {
		if (!savedSiteConfig || !siteConfig) {
			return;
		}

		const patch = createPatch(savedSiteConfig, siteConfig);

		if (!patch) {
			return;
		}

		showLoading('Saving authentication configuration...');

		try {
			await updateSiteConfig(patch);
			await siteConfigQuery.refetch();
		} catch {
			showError('Failed to save authentication configuration. Please try again.');
			return;
		}

		showSuccess('Authentication configuration saved successfully.');
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
			<!-- Header -->
			<div class="flex items-center gap-3">
				<div class="rounded-xl bg-primary/10 p-3 text-primary">
					<LockKeyhole class="size-6" />
				</div>

				<div>
					<h2 class="card-title">Authentication</h2>

					<p class="text-sm text-base-content/60">
						Configure password protection for the device web interface.
					</p>
				</div>
			</div>

			<!-- Password Protection -->
			<div class="flex items-center justify-between">
				<div class="flex flex-col">
					<span class="font-medium">Password Protection</span>

					<span class="text-sm text-base-content/50">
						Require a password when accessing the device web interface.
					</span>
				</div>

				<input
					type="checkbox"
					class="toggle toggle-primary"
					bind:checked={siteConfig.passwordEnabled}
				/>
			</div>

			<!-- Password -->
			{#if siteConfig.passwordEnabled && siteConfig.hasPassword}
				<div transition:slide class="flex flex-col overflow-hidden">
					<div class="flex flex-col">
						<span class="font-medium">Password</span>

						<span class="text-sm text-base-content/50">
							The password required to access the device web interface.
						</span>
					</div>

					<div class="join">
						<label class="input join-item w-full">
							<KeyRound class="size-4 opacity-60" />

							<input
								class="grow"
								type={showPassword ? 'text' : 'password'}
								placeholder={siteConfig.hasPassword ? 'Password is configured' : 'Enter password'}
								bind:value={siteConfig.password}
								minlength="1"
								required={siteConfig.passwordEnabled && !siteConfig.hasPassword}
							/>
						</label>

						<button
							type="button"
							class="btn join-item w-15 border-base-content/20 btn-outline"
							onclick={() => (showPassword = !showPassword)}
							aria-label={showPassword ? 'Hide password' : 'Show password'}
						>
							{#if showPassword}
								<EyeOff class="size-4" />
							{:else}
								<Eye class="size-4" />
							{/if}
						</button>
					</div>

					{#if siteConfig.hasPassword && siteConfig.password === ''}
						<span class="mt-1 text-xs text-base-content/60">
							A password is already configured. Leave this field empty to keep the current password.
						</span>
					{/if}
				</div>
			{/if}

			<!-- Actions -->
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
