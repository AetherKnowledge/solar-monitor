import { apiFetch } from '$lib/common/CommonFunctions';
import { createQuery } from '@tanstack/svelte-query';
import type { SiteConfig } from './AuthTypes';

export function createSiteConfigQuery() {
	return createQuery(() => ({
		queryKey: ['site-config'],
		queryFn: async () => await apiFetch<SiteConfig>('/api/auth/config')
	}));
}

export async function updateSiteConfig(patch: Record<string, unknown>) {
	return apiFetch<SiteConfig>('/api/auth/config', {
		method: 'POST',
		headers: {
			'Content-Type': 'application/json'
		},
		body: JSON.stringify(patch)
	});
}
