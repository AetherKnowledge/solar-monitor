<script lang="ts">
	import { resolve } from '$app/paths';
	import type { RouteId } from '$app/types';
	import MqttFillIcon from '@iconify-svelte/mingcute/mqtt-fill';
	import {
		CloudDownload,
		LayoutDashboard,
		LogOut,
		Moon,
		PanelLeftClose,
		PanelLeftOpen,
		RadioReceiver,
		ShieldLock,
		SolarPanel,
		SquareTerminal,
		SunMedium,
		Wifi
	} from '@lucide/svelte';
	import type { Component } from 'svelte';

	type StaticRoute = Exclude<RouteId, '/(app)/devices/[id]' | '/(app)/test/[id]'>;

	type SidebarItemProps = {
		label: string;
		href: StaticRoute;
		icon: typeof LayoutDashboard | typeof MqttFillIcon | Component;
	};

	let siteConfigQuery = createSiteConfigQuery();

	const sidebar = [
		{
			label: 'Dashboard',
			href: '/',
			icon: LayoutDashboard
		},
		{
			label: 'Wi-Fi',
			href: '/(app)/wifi',
			icon: Wifi
		},
		{
			label: 'MQTT',
			href: '/(app)/mqtt',
			icon: MqttFillIcon
		},
		{
			label: 'Devices',
			href: '/(app)/devices',
			icon: RadioReceiver
		},
		{
			label: 'Update',
			href: '/(app)/update',
			icon: CloudDownload
		},
		{
			label: 'Logs',
			href: '/(app)/logs',
			icon: SquareTerminal
		},
		{
			label: 'Auth',
			href: '/(app)/auth',
			icon: ShieldLock
		}
	] satisfies SidebarItemProps[];

	let drawerOpen = $state(false);
	let { children } = $props();

	import { goto } from '$app/navigation';
	import { page } from '$app/state';
	import { createSiteConfigQuery } from '$lib/auth/AuthQueries';
	import { logout } from '$lib/dashboard/DashboardController.svelte';
	import { showError, showLoading } from '$lib/popup/Popup.svelte';
	const currentTitle = $derived.by(() => {
		const segment = page.url.pathname.split('/')[1];

		if (!segment) return 'Dashboard';

		return segment.charAt(0).toUpperCase() + segment.slice(1);
	});
</script>

<div class="drawer h-screen overflow-hidden lg:drawer-open">
	<input id="my-drawer-4" type="checkbox" class="drawer-toggle" bind:checked={drawerOpen} />
	<div class="drawer-content flex h-screen flex-col">
		<!-- Navbar -->
		<nav class="navbar z-11 w-full bg-base-300">
			<label for="my-drawer-4" aria-label="open sidebar" class="btn btn-square btn-ghost">
				<span class="relative block size-5">
					<PanelLeftOpen
						class={`absolute inset-0 transition-opacity duration-200 ${
							drawerOpen ? 'opacity-0' : 'opacity-100'
						}`}
					/>

					<PanelLeftClose
						class={`absolute inset-0 transition-opacity duration-200 ${
							drawerOpen ? 'opacity-100' : 'opacity-0'
						}`}
					/>
				</span>
			</label>
			<div class="px-2 text-xl">{currentTitle}</div>
			<div class="navbar-end w-full">
				<label class="swap swap-rotate">
					<!-- this hidden checkbox controls the state -->
					<input type="checkbox" class="theme-controller" value="winter" />

					<!-- sun icon -->
					<SunMedium class="swap-off h-10 w-10" />

					<!-- moon icon -->
					<Moon class="swap-on h-10 w-10" />
				</label>
			</div>
		</nav>
		<!-- Page content here -->
		{@render children()}
	</div>

	<div class="drawer-side is-drawer-close:overflow-visible">
		<label for="my-drawer-4" aria-label="close sidebar" class="drawer-overlay"></label>
		<div
			class="flex h-screen flex-col items-start bg-base-200 is-drawer-close:w-14 is-drawer-open:w-64"
		>
			<!-- Sidebar content here -->
			<ul class="menu w-full grow">
				<!-- List item -->

				<div
					class={`flex items-center py-3 pb-4 font-bold ${
						drawerOpen ? 'gap-2 px-4' : 'px-1'
					} transition-all duration-300 ease-in-out`}
				>
					<SolarPanel class="size-8 shrink-0" />

					<span
						class={`overflow-hidden text-2xl whitespace-nowrap transition-all duration-300 ease-in-out ${
							drawerOpen ? 'ml-2 max-w-48 opacity-100' : 'ml-0 max-w-0 opacity-0'
						}`}
					>
						Solar Monitor
					</span>
				</div>

				{#each sidebar as item (item.href)}
					{@render SidebarItem(item)}
				{/each}

				{#if siteConfigQuery.data?.passwordEnabled && siteConfigQuery.data?.hasPassword}
					<li class="mt-auto">
						<button
							class="text-nowrap is-drawer-close:tooltip is-drawer-close:tooltip-right"
							data-tip="Logout"
							onclick={() => {
								showLoading('Logging out...');

								try {
									logout();
								} catch (e) {
									console.error(e);
									showError('Logout failed');
								}

								goto(resolve('/(auth)/login'));
							}}
						>
							<LogOut class="my-1.5 inline-block size-5" />

							<span class="is-drawer-close:hidden">Logout</span>
						</button>
					</li>
				{/if}
			</ul>
		</div>
	</div>
</div>

{#snippet SidebarItem({ label, href, icon: Icon }: SidebarItemProps)}
	<li>
		<a
			href={resolve(href)}
			class="text-nowrap is-drawer-close:tooltip is-drawer-close:tooltip-right"
			data-tip={label}
		>
			<Icon class="my-1.5 inline-block size-5" />

			<span class="is-drawer-close:hidden">{label}</span>
		</a>
	</li>
{/snippet}
