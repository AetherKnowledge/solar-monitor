import { apiFetch } from '$lib/common/CommonFunctions';
import { createQuery } from '@tanstack/svelte-query';

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

	return {
		query
	};
}

export function updateFirmware(
	file: File,
	onProgress?: (progress: number) => void
): Promise<boolean> {
	return new Promise((resolve) => {
		const xhr = new XMLHttpRequest();

		xhr.open('POST', '/api/update/firmware');

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

export async function updateWebsite(
	file: File,
	onProgress?: (progress: number) => void
): Promise<boolean> {
	return new Promise((resolve) => {
		const xhr = new XMLHttpRequest();

		xhr.open('POST', '/api/update/website');

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

export async function checkForUpdates() {
	await new Promise((resolve) => setTimeout(resolve, 5000));

	// Fake API response
	latestVersion.firmware = '1.2.4';
	latestVersion.website = '1.2.4';
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
