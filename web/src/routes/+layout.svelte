<script lang="ts">
	import { resolve } from '$app/paths';
	import type { RouteId } from '$app/types';
	import favicon from '$lib/assets/favicon.svg';
	import PopupHost from '$lib/popup/PopupHost.svelte';
	import { queryClient } from '$lib/queryClient';
	import MqttFillIcon from '@iconify-svelte/mingcute/mqtt-fill';
	import {
		CloudDownload,
		LayoutDashboard,
		PanelLeftClose,
		PanelLeftOpen,
		RadioReceiver,
		SolarPanel,
		SquareTerminal,
		Wifi
	} from '@lucide/svelte';
	import { QueryClientProvider } from '@tanstack/svelte-query';
	import type { Component } from 'svelte';
	import { cubicOut } from 'svelte/easing';
	import { fly } from 'svelte/transition';
	import './layout.css';

	type StaticRoute = Exclude<RouteId, '/devices/[id]' | '/test/[id]'>;

	type SidebarItemProps = {
		label: string;
		href: StaticRoute;
		icon: typeof LayoutDashboard | typeof MqttFillIcon | Component;
	};

	const sidebar = [
		{
			label: 'Dashboard',
			href: '/',
			icon: LayoutDashboard
		},
		{
			label: 'Wi-Fi',
			href: '/wifi',
			icon: Wifi
		},
		{
			label: 'MQTT',
			href: '/mqtt',
			icon: MqttFillIcon
		},
		{
			label: 'Devices',
			href: '/devices',
			icon: RadioReceiver
		},
		{
			label: 'Update',
			href: '/update',
			icon: CloudDownload
		},
		{
			label: 'Logs',
			href: '/logs',
			icon: SquareTerminal
		}
	] satisfies SidebarItemProps[];

	let drawerOpen = $state(false);
	let { children } = $props();

	import { page } from '$app/state';
	const currentTitle = $derived.by(() => {
		const segment = page.url.pathname.split('/')[1];

		if (!segment) return 'Dashboard';

		return segment.charAt(0).toUpperCase() + segment.slice(1);
	});
</script>

<svelte:head>
	<link rel="icon" href={favicon} />
	<link rel="preload" href="/roboto.woff2" as="font" type="font/woff2" crossorigin="anonymous" />
</svelte:head>
<QueryClientProvider client={queryClient}>
	<div class="drawer h-screen overflow-hidden lg:drawer-open">
		<input id="my-drawer-4" type="checkbox" class="drawer-toggle" bind:checked={drawerOpen} />
		<div class="drawer-content flex h-screen flex-col">
			<!-- Navbar -->
			<nav class="navbar z-11 w-full bg-base-300">
				<label for="my-drawer-4" aria-label="open sidebar" class="btn btn-square btn-ghost">
					<!-- Sidebar toggle icon -->
					{#key drawerOpen}
						<span
							transition:fly={{
								x: drawerOpen ? -8 : 8,
								opacity: 0.3,
								duration: 180,
								easing: cubicOut
							}}
						>
							{#if drawerOpen}
								<PanelLeftClose class="my-1.5 inline-block size-5" />
							{:else}
								<PanelLeftOpen class="my-1.5 inline-block size-5" />
							{/if}
						</span>
					{/key}
				</label>
				<div class="px-2 text-xl">{currentTitle}</div>
				<div class="navbar-end w-full">
					<label class="swap swap-rotate">
						<!-- this hidden checkbox controls the state -->
						<input type="checkbox" class="theme-controller" value="winter" />

						<!-- sun icon -->
						<svg
							class="swap-off h-10 w-10 fill-current"
							xmlns="http://www.w3.org/2000/svg"
							viewBox="0 0 24 24"
						>
							<path
								d="M5.64,17l-.71.71a1,1,0,0,0,0,1.41,1,1,0,0,0,1.41,0l.71-.71A1,1,0,0,0,5.64,17ZM5,12a1,1,0,0,0-1-1H3a1,1,0,0,0,0,2H4A1,1,0,0,0,5,12Zm7-7a1,1,0,0,0,1-1V3a1,1,0,0,0-2,0V4A1,1,0,0,0,12,5ZM5.64,7.05a1,1,0,0,0,.7.29,1,1,0,0,0,.71-.29,1,1,0,0,0,0-1.41l-.71-.71A1,1,0,0,0,4.93,6.34Zm12,.29a1,1,0,0,0,.7-.29l.71-.71a1,1,0,1,0-1.41-1.41L17,5.64a1,1,0,0,0,0,1.41A1,1,0,0,0,17.66,7.34ZM21,11H20a1,1,0,0,0,0,2h1a1,1,0,0,0,0-2Zm-9,8a1,1,0,0,0-1,1v1a1,1,0,0,0,2,0V20A1,1,0,0,0,12,19ZM18.36,17A1,1,0,0,0,17,18.36l.71.71a1,1,0,0,0,1.41,0,1,1,0,0,0,0-1.41ZM12,6.5A5.5,5.5,0,1,0,17.5,12,5.51,5.51,0,0,0,12,6.5Zm0,9A3.5,3.5,0,1,1,15.5,12,3.5,3.5,0,0,1,12,15.5Z"
							/>
						</svg>

						<!-- moon icon -->
						<svg
							class="swap-on h-10 w-10 fill-current"
							xmlns="http://www.w3.org/2000/svg"
							viewBox="0 0 24 24"
						>
							<path
								d="M21.64,13a1,1,0,0,0-1.05-.14,8.05,8.05,0,0,1-3.37.73A8.15,8.15,0,0,1,9.08,5.49a8.59,8.59,0,0,1,.25-2A1,1,0,0,0,8,2.36,10.14,10.14,0,1,0,22,14.05,1,1,0,0,0,21.64,13Zm-9.5,6.69A8.14,8.14,0,0,1,7.08,5.22v.27A10.15,10.15,0,0,0,17.22,15.63a9.79,9.79,0,0,0,2.1-.22A8.11,8.11,0,0,1,12.14,19.73Z"
							/>
						</svg>
					</label>
				</div>
			</nav>
			<!-- Page content here -->
			<main class="flex h-full flex-1 overflow-auto p-4">
				<div class="flex min-w-0 flex-1 flex-col">
					{@render children()}
					<PopupHost />
				</div>
			</main>
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
						class={`flex items-center py-2 font-bold ${
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
				</ul>
			</div>
		</div>
	</div>
</QueryClientProvider>
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
