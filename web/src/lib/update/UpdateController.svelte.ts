import { apiFetch } from '$lib/common/CommonFunctions';
import type { SimpleResponse } from '$lib/common/CommonTypes';

export type VersionInfo = {
	version: string;
	latest: string;
};

export type AppVersion = {
	firmware: VersionInfo;
	website: VersionInfo;
};

export const currentVersion: AppVersion = $state({
	firmware: {
		version: 'v1.2.3',
		latest: 'v1.2.3'
	},
	website: {
		version: 'v1.1.8',
		latest: 'v1.1.8'
	}
});

export async function updateFirmware(file: File): Promise<boolean> {
	return await apiFetch<SimpleResponse>(`/api/update/firmware`, {
		method: 'POST',
		headers: {
			'Content-Type': 'application/octet-stream'
		},
		body: file
	})
		.then(() => true)
		.catch(() => false);
}

export async function updateWebsite(file: File): Promise<boolean> {
	return await apiFetch<SimpleResponse>(`/api/update/website`, {
		method: 'POST',
		headers: {
			'Content-Type': 'application/octet-stream'
		},
		body: file
	})
		.then(() => true)
		.catch(() => false);
}

export async function checkForUpdates() {
	await new Promise((resolve) => setTimeout(resolve, 5000));

	// Fake API response
	currentVersion.firmware = {
		version: 'v1.2.3',
		latest: 'v1.3.0'
	};

	currentVersion.website = {
		version: 'v1.1.8',
		latest: 'v1.2.3'
	};
}

function parseVersion(version: string): number[] {
	const versionParts = version.replace(/^v/, '').split('.');
	return versionParts.map((part) => parseInt(part, 10));
}

export function isUpToDate(versionInfo: VersionInfo): boolean {
	const currentVersionNumber = parseVersion(versionInfo.version);
	const latestVersionNumber = parseVersion(versionInfo.latest);
	return currentVersionNumber.every((part, index) => part >= latestVersionNumber[index]);
}
