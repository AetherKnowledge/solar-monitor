<script lang="ts">
	import type { Snippet } from 'svelte';
	type Props = {
		children?: Snippet;
	};

	let { children }: Props = $props();
</script>

<div class="fixed inset-0 bg-base-100/70 backdrop-blur-sm z-10">
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
			{#if children}
				{@render children()}
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
