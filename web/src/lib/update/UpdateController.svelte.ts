import { apiFetch } from '$lib/common/CommonFunctions';
import { createQuery } from '@tanstack/svelte-query';
import { onMount } from 'svelte';

export type ReleaseAsset = {
	version: string;
	file: string;
	url: string;
	size: number;
	sha256: string;
};

export type Manifest = {
	version: string;
	buildTime: string;

	firmware: ReleaseAsset;
	website: ReleaseAsset;
};

export type VersionApiResponse = {
	firmware: string;
	website: string;
};

export const latestVersion = $state<VersionApiResponse>({
	firmware: '0.0.0',
	website: __WEBSITE_VERSION__
});

export function createUpdateController() {
	const query = createQuery<VersionApiResponse>(() => ({
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

	onMount(() => {
		checkForUpdates();
	});

	return {
		query
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

export async function checkForUpdates(): Promise<Manifest> {
	const manifest = await apiFetch<Manifest>(
		'https://raw.githubusercontent.com/AetherKnowledge/solar-monitor/main/manifest.json',
		{
			cache: 'no-store'
		}
	);

	latestVersion.firmware = manifest.firmware.version;
	latestVersion.website = manifest.website.version;

	return manifest;
}

export async function updateFirmware(): Promise<void> {
	const manifest = await apiFetch<Manifest>(
		'https://raw.githubusercontent.com/AetherKnowledge/solar-monitor/main/manifest.json',
		{
			cache: 'no-store'
		}
	);

	await apiFetch('/api/update/firmware', {
		method: 'POST',
		headers: {
			'Content-Type': 'application/json'
		},
		body: JSON.stringify(manifest.firmware)
	});
}

export async function updateWebsite(): Promise<void> {
	const manifest = await apiFetch<Manifest>(
		'https://raw.githubusercontent.com/AetherKnowledge/solar-monitor/main/manifest.json',
		{
			cache: 'no-store'
		}
	);

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
