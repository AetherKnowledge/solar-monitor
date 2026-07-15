<script lang="ts">
	type Props = {
		text?: string;
		progress?: number | null;
		showProgress?: boolean;
	};

	let { text = 'Loading...', progress = null, showProgress = false }: Props = $props();
</script>

<div class="relative flex h-full items-center justify-center overflow-hidden">
	<!-- Background -->
	<div class="absolute inset-0 overflow-hidden">
		<div class="blob blob-1"></div>
		<div class="blob blob-2"></div>
		<div class="blob blob-3"></div>
	</div>

	<!-- Card -->
	<div
		class="card bg-base-100/80 backdrop-blur-xl shadow-2xl border border-base-300 w-96 animate-fade-up"
	>
		<div class="card-body items-center text-center gap-6">
			<div class="relative">
				<div class="loading loading-ring loading-xl text-primary"></div>

				<div class="absolute inset-0 flex items-center justify-center animate-pulse">
					<div class="size-3 rounded-full bg-primary"></div>
				</div>
			</div>

			<div class="space-y-2">
				<h2 class="text-xl font-bold">{text}</h2>

				<p class="text-sm text-base-content/60">Please wait while everything is prepared.</p>
			</div>

			{#if showProgress}
				<div class="w-full space-y-2">
					<progress class="progress progress-primary w-full" value={progress ?? 0} max="100"
					></progress>

					<p class="text-xs text-base-content/60">
						{progress ?? 0}%
					</p>
				</div>
			{/if}
		</div>
	</div>
</div>

<style>
	.blob {
		position: absolute;
		border-radius: 9999px;
		filter: blur(70px);
		opacity: 0.18;
		animation: float 10s ease-in-out infinite;
	}

	.blob-1 {
		width: 18rem;
		height: 18rem;
		background: oklch(var(--p));
		top: -4rem;
		left: -3rem;
	}

	.blob-2 {
		width: 24rem;
		height: 24rem;
		background: oklch(var(--s));
		bottom: -8rem;
		right: -5rem;
		animation-delay: -3s;
	}

	.blob-3 {
		width: 14rem;
		height: 14rem;
		background: oklch(var(--a));
		top: 40%;
		left: 55%;
		animation-delay: -6s;
	}

	@keyframes float {
		0%,
		100% {
			transform: translate(0, 0) scale(1);
		}
		50% {
			transform: translate(30px, -40px) scale(1.1);
		}
	}

	.animate-fade-up {
		animation: fadeUp 0.45s ease-out;
	}

	@keyframes fadeUp {
		from {
			opacity: 0;
			transform: translateY(20px) scale(0.98);
		}
		to {
			opacity: 1;
			transform: translateY(0) scale(1);
		}
	}
</style>
