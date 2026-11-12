import { apiFetch } from '$lib/common/CommonFunctions';
import { UpdateStatus } from '$lib/common/CommonTypes';
import { hidePopup, showError, showLoading } from '$lib/popup/Popup.svelte';
import { createQuery } from '@tanstack/svelte-query';

export type UpdateProgress = {
	status: UpdateStatus;
	progress: number;
	hasTotalSize: boolean;
};

export type ReleaseAsset = {
	version: string;
	file: string;
	url: string;
	size: number;
	sha256: string;
};

export type BoardManifest = {
	firmware: ReleaseAsset;
	website: ReleaseAsset;
};

export type Manifest = {
	buildTime: string;
	boards: Record<string, BoardManifest>;
};

export type VersionApiResponse = {
	firmware: string;
	website: string;
};

export const latestVersion = $state<VersionApiResponse>({
	firmware: '0.0.0',
	website: __WEBSITE_VERSION__
});

export async function getBoardManifest(): Promise<BoardManifest> {
	const manifestResponse = await apiFetch<Manifest>(
		'https://raw.githubusercontent.com/AetherKnowledge/solar-monitor/main/manifest.json',
		{
			cache: 'no-store'
		}
	);
	const boardResponse = await apiFetch<{ boardType: string }>('/api/system/board');

	if (!boardResponse?.boardType) {
		throw new Error('Device did not report its board type. Update the firmware and try again.');
	}

	const boardManifest = manifestResponse?.boards?.[boardResponse.boardType];
	if (!boardManifest?.firmware || !boardManifest?.website) {
		throw new Error(`No update is available for board "${boardResponse.boardType}".`);
	}

	return boardManifest;
}

export function createUpdateController() {
	const versionQuery = createQuery<VersionApiResponse>(() => ({
		queryKey: ['version'],
		queryFn: async () => apiFetch(`/api/version`),
		initialData: {
			firmware: '0.0.0',
			website: __WEBSITE_VERSION__
		},
		refetchInterval: 60000,
		initialDataUpdatedAt: Date.now(),
		staleTime: 0
	}));

	const statusQuery = createQuery<UpdateProgress>(() => ({
		queryKey: ['updateStatus'],
		queryFn: async () => apiFetch(`/api/update/status`),
		initialData: { status: UpdateStatus.NotStarted, progress: 0, hasTotalSize: false },
		initialDataUpdatedAt: Date.now(),
		staleTime: 0,
		refetchInterval: (query) => {
			const status = query.state.data?.status;
			if (status === UpdateStatus.InProgress || status === UpdateStatus.Requested) {
				return 1000;
			}
		}
	}));

	let previousStatus = UpdateStatus.NotStarted;
	$effect(() => {
		const { status, hasTotalSize } = statusQuery.data;
		const statusChanged = status !== previousStatus;
		const wasUpdating =
			previousStatus === UpdateStatus.InProgress || previousStatus === UpdateStatus.Requested;
		previousStatus = status;

		// let the manual update do the loading progress
		if (hasTotalSize) {
			return;
		}

		if (status === UpdateStatus.InProgress || status === UpdateStatus.Requested) {
			showLoading('Installing update...');
		} else if (status === UpdateStatus.UpdateFailed && statusChanged) {
			showError('Update failed. Please check the device logs and try again.');
		} else if (statusChanged && (wasUpdating || status === UpdateStatus.UpdateComplete)) {
			hidePopup();
		}
	});

	return {
		versionQuery,
		statusQuery
	};
}

export function updateFirmwareManual(
	file: File,
	onProgress?: (progress: number) => void
): Promise<boolean> {
	return new Promise((resolve) => {
		const xhr = new XMLHttpRequest();

		xhr.open('POST', '/api/update/firmware/manual');

		xhr.setRequestHeader('Content-Type', 'application/octet-stream');

		xhr.upload.onprogress = (event) => {
			if (!event.lengthComputable) return;

			const progress = (event.loaded / event.total) * 100;

			onProgress?.(progress);
		};

		xhr.onload = () => {
			resolve(xhr.status >= 200 && xhr.status < 300);
		};

		xhr.onerror = () => {
			resolve(false);
		};

		xhr.onabort = () => {
			resolve(false);
		};

		xhr.send(file);
	});
}

export async function updateWebsiteManual(
	file: File,
	onProgress?: (progress: number) => void
): Promise<boolean> {
	return new Promise((resolve) => {
		const xhr = new XMLHttpRequest();

		xhr.open('POST', '/api/update/website/manual');

		xhr.setRequestHeader('Content-Type', 'application/octet-stream');

		xhr.upload.onprogress = (event) => {
			if (!event.lengthComputable) return;

			const progress = (event.loaded / event.total) * 100;

			onProgress?.(progress);
		};

		xhr.onload = () => {
			resolve(xhr.status >= 200 && xhr.status < 300);
		};

		xhr.onerror = () => {
			resolve(false);
		};

		xhr.onabort = () => {
			resolve(false);
		};

		xhr.send(file);
	});
}

export async function checkForUpdates(): Promise<BoardManifest> {
	const manifest = await getBoardManifest();

	latestVersion.firmware = manifest.firmware.version;
	latestVersion.website = manifest.website.version;

	return manifest;
}

export async function updateFirmware(): Promise<void> {
	const manifest = await getBoardManifest();

	await apiFetch('/api/update/firmware', {
		method: 'POST',
		headers: {
			'Content-Type': 'application/json'
		},
		body: JSON.stringify(manifest.firmware)
	});
}

export async function updateWebsite(): Promise<void> {
	const manifest = await getBoardManifest();

	await apiFetch('/api/update/website', {
		method: 'POST',
		headers: {
			'Content-Type': 'application/json'
		},
		body: JSON.stringify(manifest.website)
	});
}

function parseVersion(version: string): number[] {
	return version
		.replace(/^v/, '')
		.split('.')
		.map((part) => parseInt(part, 10));
}

export function isUpToDate(version: string, latestVersion: string): boolean {
	const current = parseVersion(version);
	const latest = parseVersion(latestVersion);

	const length = Math.max(current.length, latest.length);

	for (let i = 0; i < length; i++) {
		const currentPart = current[i] ?? 0;
		const latestPart = latest[i] ?? 0;

		if (currentPart > latestPart) {
			return true;
		}

		if (currentPart < latestPart) {
			return false;
		}
	}

	return true;
}

export async function testUpdate(
	_file: File,
	onProgress?: (progress: number) => void
): Promise<boolean> {
	let progress = 0;

	onProgress?.(progress);

	while (progress < 100) {
		await new Promise((resolve) => setTimeout(resolve, 5));

		progress += Math.random() * 8 + 1;
		progress = Math.min(progress, 100);

		onProgress?.(progress);
	}

	await new Promise((resolve) => setTimeout(resolve, 500));

	return true;
}
