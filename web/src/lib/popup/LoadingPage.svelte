<script lang="ts">
	import BlurPage from './BlurPage.svelte';
	import PopupCard from './PopupCard.svelte';

	type Props = {
		text?: string;
		progress?: number | null;
		showProgress?: boolean;
	};

	let { text = 'Loading...', progress = null, showProgress = false }: Props = $props();
</script>

<BlurPage>
	<PopupCard>
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
	</PopupCard>
</BlurPage>
