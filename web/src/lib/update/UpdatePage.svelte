<script lang="ts">
	import { hidePopup } from '$lib/popup/Popup.svelte';
	import {
		ArrowUpCircle,
		CloudDownload,
		Download,
		Globe,
		HardDriveUpload,
		Microchip,
		RefreshCw
	} from '@lucide/svelte';
	import { onMount } from 'svelte';
	import {
		checkForUpdates,
		currentVersion,
		isUpToDate,
		updateFirmware,
		updateWebsite
	} from './UpdateController.svelte';

	let checkingForUpdates = $state(false);
	let firmwareFiles = $state<FileList>();
	let websiteFiles = $state<FileList>();

	let newFirmwareFile = $derived(firmwareFiles?.[0] ?? null);
	let newWebsiteFile = $derived(websiteFiles?.[0] ?? null);

	async function onCheckUpdate() {
		if (checkingForUpdates) return;

		checkingForUpdates = true;
		await checkForUpdates();
		checkingForUpdates = false;
	}

	async function onUpdateFirmware() {
		console.log('Updating firmware with file:', newFirmwareFile);

		if (!newFirmwareFile) return;

		const result = await updateFirmware(newFirmwareFile);
		console.log('Firmware update result:', result);
	}

	async function onUpdateWebsite() {
		if (!newWebsiteFile) return;

		const result = await updateWebsite(newWebsiteFile);
		console.log('Website update result:', result);
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
				<ArrowUpCircle class="size-7" />
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
			<div class="rounded-xl border border-base-300 bg-base-200/40 p-5">
				<div class="flex items-start justify-between gap-4">
					<div class="flex items-center gap-3">
						<div class="rounded-lg bg-primary/10 p-2 text-primary">
							<Microchip class="size-5" />
						</div>

						<div>
							<div class="text-sm text-base-content/60">Firmware Version</div>

							<div class="font-mono text-lg font-semibold">
								{currentVersion.firmware.version}
							</div>

							{#if !isUpToDate(currentVersion.firmware)}
								<div class="mt-1 text-sm text-base-content/60">
									Latest:
									<span class="font-mono">
										{currentVersion.firmware.latest}
									</span>
								</div>
							{/if}
						</div>
					</div>

					<div class="flex flex-col items-end gap-3">
						{#if isUpToDate(currentVersion.firmware)}
							<div class="badge gap-1 badge-success">
								<span class="size-2 rounded-full bg-current"></span>
								Up to date
							</div>
						{:else}
							<div class="badge gap-1 badge-warning">
								<span class="size-2 rounded-full bg-current"></span>
								Update Available
							</div>

							<div class="tooltip tooltip-left" data-tip="Download update">
								<button
									class="btn btn-square btn-ghost btn-primary btn-sm"
									aria-label="Download update"
								>
									<Download class="size-4" />
								</button>
							</div>
						{/if}
					</div>
				</div>
			</div>

			<div class="rounded-xl border border-base-300 bg-base-200/40 p-5">
				<div class="flex items-start justify-between gap-4">
					<div class="flex items-center gap-3">
						<div class="rounded-lg bg-primary/10 p-2 text-primary">
							<Globe class="size-5" />
						</div>

						<div>
							<div class="text-sm text-base-content/60">Web Interface Version</div>

							<div class="font-mono text-lg font-semibold">
								{currentVersion.website.version}
							</div>

							{#if !isUpToDate(currentVersion.website)}
								<div class="mt-1 text-sm text-base-content/60">
									Latest:
									<span class="font-mono">
										{currentVersion.website.latest}
									</span>
								</div>
							{/if}
						</div>
					</div>

					<div class="flex flex-col items-end gap-3">
						{#if isUpToDate(currentVersion.website)}
							<div class="badge gap-1 badge-success">
								<span class="size-2 rounded-full bg-current"></span>
								Up to date
							</div>
						{:else}
							<div class="badge gap-1 badge-warning">
								<span class="size-2 rounded-full bg-current"></span>
								Update Available
							</div>

							<div class="tooltip tooltip-left" data-tip="Download update">
								<button
									class="btn btn-square btn-ghost btn-primary btn-sm"
									aria-label="Download update"
								>
									<Download class="size-4" />
								</button>
							</div>
						{/if}
					</div>
				</div>
			</div>
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
					accept=".bin,.zip,.gz"
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
