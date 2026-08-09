import { apiFetch } from '$lib/common/CommonFunctions';
import type { SimpleResponse } from '$lib/common/CommonTypes';
import { createQuery } from '@tanstack/svelte-query';
import { defaultSystemState, type SystemState } from './DashboardTypes';

export function createDashboardController() {
	const query = createQuery<SystemState>(() => ({
		queryKey: ['systemValues'],
		queryFn: async () => apiFetch(`/api/system`),
		initialData: defaultSystemState,
		refetchInterval: 5000,
		initialDataUpdatedAt: Date.now(),
		staleTime: 0
	}));
	return {
		query
	};
}

export function logout() {
	return apiFetch<SimpleResponse>(`/api/auth/logout`, {
		method: 'POST'
	});
}
