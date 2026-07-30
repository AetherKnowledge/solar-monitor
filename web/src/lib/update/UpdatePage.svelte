<script lang="ts">
	import {
		hidePopup,
		showError,
		showLoading,
		showSuccess,
		showYesNo,
		updateLoadingProgress
	} from '$lib/popup/Popup.svelte';
	import {
		CircleArrowUp,
		CloudDownload,
		Download,
		Globe,
		HardDriveUpload,
		Microchip,
		RefreshCw
	} from '@lucide/svelte';
	import { type Component, onMount } from 'svelte';
	import {
		checkForUpdates,
		createUpdateController,
		isUpToDate,
		latestVersion,
		updateFirmware,
		updateWebsite
	} from './UpdateController.svelte';

	const { query } = createUpdateController();

	let checkingForUpdates = $state(false);
	let firmwareFiles = $state<FileList>();
	let websiteFiles = $state<FileList>();

	let newFirmwareFile = $derived(firmwareFiles?.[0] ?? null);
	let newWebsiteFile = $derived(websiteFiles?.[0] ?? null);
	let currentVersion = $derived(query.data);

	async function onCheckUpdate() {
		if (checkingForUpdates) return;

		checkingForUpdates = true;
		await checkForUpdates();
		checkingForUpdates = false;
	}

	async function onUpdateFirmware() {
		if (!newFirmwareFile) return;

		if (
			!(await showYesNo({
				message:
					'Are you sure you want to update the firmware? This will overwrite the existing firmware.',
				hintText:
					'This process is not reversible. Make sure to backup your config files before proceeding.',
				warning: true
			}))
		) {
			return;
		}

		showLoading('Uploading firmware...', 0);

		const result = await updateFirmware(newFirmwareFile, (progress) => {
			updateLoadingProgress(progress);
		});

		if (!result) {
			showError('Failed to update firmware. Please try again.');
			return;
		}

		firmwareFiles = undefined;
		showSuccess('Firmware updated successfully.');
	}

	async function onUpdateWebsite() {
		if (!newWebsiteFile) return;

		if (
			!(await showYesNo({
				message:
					'Are you sure you want to update the web interface? This will overwrite the existing files.',
				hintText:
					'This process is not reversible. Make sure to backup your config files before proceeding.',
				warning: true
			}))
		) {
			return;
		}

		showLoading('Uploading web interface...', 0);

		const result = await updateWebsite(newWebsiteFile, (progress) => {
			updateLoadingProgress(progress);
		});

		if (!result) {
			showError('Failed to update the web interface. Please try again.');
			return;
		}

		websiteFiles = undefined;
		showSuccess('Web interface updated successfully.');
	}

	onMount(() => {
		return () => {
			hidePopup();
		};
	});
</script>

<div class="card border border-base-300 bg-base-100 shadow-sm">
	<div class="card-body gap-6">
		<!-- Header -->

		<div class="flex items-center gap-3">
			<div class="rounded-xl bg-primary/10 p-3 text-primary">
				<CircleArrowUp class="size-7" />
			</div>

			<div>
				<h2 class="card-title">Software Updates</h2>

				<p class="text-sm text-base-content/60">
					Keep your device up to date with the latest firmware and web interface.
				</p>
			</div>
		</div>

		<div class="divider my-0"></div>

		<!-- Installed Versions -->

		<div class="flex items-center gap-3">
			<div class="rounded-xl bg-secondary/10 p-3 text-secondary">
				<CloudDownload class="size-6" />
			</div>

			<div>
				<h2 class="card-title">Installed Versions</h2>

				<p class="text-sm text-base-content/60">Current software installed on this device.</p>
			</div>
		</div>

		<div class="grid gap-5 md:grid-cols-2">
			{@render versionCard(
				Microchip,
				'Firmware Version',
				currentVersion.firmware,
				latestVersion.firmware,
				'Download firmware'
			)}

			{@render versionCard(
				Globe,
				'Web Interface Version',
				currentVersion.website,
				latestVersion.website,
				'Download website'
			)}
		</div>

		<button class="btn btn-primary" onclick={onCheckUpdate} disabled={checkingForUpdates}>
			{#if checkingForUpdates}
				<span class="loading loading-sm loading-spinner"></span>
				Checking...
			{:else}
				<RefreshCw class="size-4" />
				Check for Updates
			{/if}
		</button>

		<div class="divider my-0"></div>

		<!-- Manual Upload -->

		<div class="flex items-center gap-3">
			<div class="rounded-xl bg-success/10 p-3 text-success">
				<HardDriveUpload class="size-6" />
			</div>

			<div>
				<h2 class="card-title">Manual Update</h2>

				<p class="text-sm text-base-content/60">Install firmware or the web interface manually.</p>
			</div>
		</div>

		<div class="grid gap-5 lg:grid-cols-2">
			<div class="rounded-xl border border-base-300 p-5">
				<h3 class="font-semibold">Firmware</h3>

				<p class="mt-1 mb-4 text-sm text-base-content/60">
					Upload a compiled firmware binary (.bin).
				</p>

				<input
					bind:files={firmwareFiles}
					type="file"
					class="file-input-bordered file-input w-full"
					accept=".bin"
				/>

				<button
					class="btn mt-4 w-full btn-primary"
					disabled={!newFirmwareFile}
					onclick={onUpdateFirmware}
				>
					Upload Firmware
				</button>
			</div>

			<div class="rounded-xl border border-base-300 p-5">
				<h3 class="font-semibold">Web Interface</h3>

				<p class="mt-1 mb-4 text-sm text-base-content/60">
					Upload the compressed web interface package.
				</p>

				<input
					bind:files={websiteFiles}
					type="file"
					class="file-input-bordered file-input w-full"
					accept=".bin"
				/>

				<button
					class="btn mt-4 w-full btn-primary"
					disabled={!newWebsiteFile}
					onclick={onUpdateWebsite}
				>
					Upload Website
				</button>
			</div>
		</div>
	</div>
</div>

{#snippet versionCard(
	Icon: Component,
	title: string,
	currentVersion: string,
	latestVersion: string,
	downloadTooltip: string,
	onDownload?: () => void
)}
	<div class="rounded-xl border border-base-300 bg-base-200/40 p-5">
		<div
			class="flex flex-col gap-4
		   lg:flex-row lg:items-start lg:justify-between"
		>
			<div class="flex items-center gap-3">
				<div class="rounded-lg bg-primary/10 p-2 text-primary">
					<Icon class="size-5" />
				</div>

				<div>
					<div class="text-sm text-base-content/60">
						{title}
					</div>

					<div class="font-mono text-lg font-semibold">
						{currentVersion}
					</div>

					<div class="mt-1 text-sm text-base-content/60">
						Latest:
						<span class="font-mono">
							{latestVersion}
						</span>
					</div>
				</div>
			</div>

			<div
				class="flex flex-col gap-3 border-t border-base-300 pt-4
		   sm:border-0 sm:pt-0
		   lg:items-end"
			>
				{#if isUpToDate(currentVersion, latestVersion)}
					<div
						class="badge w-full justify-center gap-1 badge-success
			   lg:w-auto lg:self-auto"
					>
						<span class="size-2 rounded-full bg-current"></span>
						Up to date
					</div>
				{:else}
					<div
						class="badge w-full justify-center gap-1 badge-warning
			   lg:w-auto lg:self-auto"
					>
						<span class="size-2 rounded-full bg-current"></span>
						Update Available
					</div>

					<button class="btn w-full btn-primary lg:hidden" onclick={onDownload}>
						<Download class="size-4" />
						Update
					</button>

					<div class="tooltip tooltip-left hidden lg:block" data-tip={downloadTooltip}>
						<button
							class="btn btn-square btn-ghost btn-primary btn-sm"
							aria-label={downloadTooltip}
							onclick={onDownload}
						>
							<Download class="size-4" />
						</button>
					</div>
				{/if}
			</div>
		</div>
	</div>
{/snippet}
