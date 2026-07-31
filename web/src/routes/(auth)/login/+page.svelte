<script lang="ts">
	import { Eye, EyeOff, Lock, Moon, ShieldCheck, SunMedium } from '@lucide/svelte';

	let password = $state('');
	let loading = $state(false);
	let showPassword = $state(false);

	async function login() {
		loading = true;

		try {
			console.log({ password });
		} finally {
			loading = false;
		}
	}
</script>

<svelte:head>
	<title>Login • Solar Monitor</title>
</svelte:head>

<div
	class="relative flex min-h-screen items-center justify-center overflow-hidden bg-linear-to-br from-base-300 via-base-200 to-base-100 p-6"
>
	<!-- Background Glow -->
	<div
		class="absolute top-0 left-1/2 h-96 w-96 -translate-x-1/2 rounded-full bg-primary/10 blur-3xl"
	></div>

	<!-- Theme Toggle -->
	<div class="absolute top-6 right-6">
		<label class="btn swap btn-circle swap-rotate btn-ghost">
			<input type="checkbox" class="theme-controller" value="winter" />

			<SunMedium class="swap-off size-5" />
			<Moon class="swap-on size-5" />
		</label>
	</div>

	<div class="relative w-full max-w-md">
		<!-- Logo -->
		<div class="mb-8 text-center">
			<div
				class="mx-auto flex size-20 items-center justify-center rounded-full bg-linear-to-br from-primary to-primary/70 text-primary-content shadow-xl shadow-primary/30"
			>
				<SunMedium class="size-9" />
			</div>

			<h1 class="mt-6 text-4xl font-bold tracking-tight">Solar Monitor</h1>

			<div class="mt-3 flex justify-center">
				<div class="badge gap-2 badge-outline px-3 py-3 badge-primary">
					<ShieldCheck class="size-3.5" />
					Local Device
				</div>
			</div>

			<p class="mt-4 text-sm text-base-content/60">
				Enter the administrator password to access this device.
			</p>
		</div>

		<!-- Login Card -->
		<div class="card border border-base-300/60 bg-base-100/80 shadow-2xl backdrop-blur-xl">
			<div class="card-body items-center gap-6 text-center">
				<div>
					<h2 class="text-xl font-semibold">Administrator Access</h2>

					<p class="mt-2 max-w-xs text-sm text-base-content/60">
						Only authorized users can modify device settings.
					</p>
				</div>

				<label class="input-bordered input flex w-full items-center gap-3">
					<Lock class="size-4 opacity-60" />

					<input
						bind:value={password}
						type={showPassword ? 'text' : 'password'}
						placeholder="Enter password"
						class="grow"
						autocomplete="current-password"
						onkeydown={(e) => e.key === 'Enter' && login()}
					/>

					<button
						type="button"
						class="btn btn-circle btn-ghost btn-xs"
						aria-label={showPassword ? 'Hide password' : 'Show password'}
						onclick={() => (showPassword = !showPassword)}
					>
						{#if showPassword}
							<EyeOff class="size-4" />
						{:else}
							<Eye class="size-4" />
						{/if}
					</button>
				</label>

				<button class="btn w-full btn-lg btn-primary" class:loading onclick={login}>
					Unlock Device
				</button>
			</div>
		</div>

		<!-- Footer -->
		<div class="mt-8 space-y-2 text-center">
			<p class="text-xs text-base-content/50">Secure local administration interface</p>

			<p class="text-xs font-medium text-base-content/40">Firmware v1.1.1</p>
		</div>
	</div>
</div>
