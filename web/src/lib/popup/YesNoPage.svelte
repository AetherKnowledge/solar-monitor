<script lang="ts">
	import { Check, CircleHelp, TriangleAlert, X } from '@lucide/svelte';
	import BlurPage from './BlurPage.svelte';
	import PopupCard from './PopupCard.svelte';

	type Props = {
		title?: string;
		message?: string;
		yesText?: string;
		noText?: string;
		warning?: boolean;
		onYes?: () => void;
		onNo?: () => void;
	};

	let {
		title = 'Confirmation',
		message = 'Are you sure you want to continue?',
		yesText = 'Yes',
		noText = 'Cancel',
		warning = false,
		onYes,
		onNo
	}: Props = $props();

	let visible = $state(true);

	function yes() {
		visible = false;
		onYes?.();
	}

	function no() {
		visible = false;
		onNo?.();
	}
</script>

{#if visible}
	<BlurPage>
		<PopupCard>
			<div class="card-body items-center text-center gap-6">
				<div
					class={`flex size-24 items-center justify-center rounded-full ${
						warning ? 'bg-warning/15 text-warning' : 'bg-info/15 text-info'
					}`}
				>
					{#if warning}
						<TriangleAlert class="size-14" />
					{:else}
						<CircleHelp class="size-14" />
					{/if}
				</div>

				<div class="space-y-2">
					<h2 class="text-2xl font-bold">{title}</h2>

					<p class="text-base-content/70 wrap-break-words">
						{message}
					</p>
				</div>

				{#if warning}
					<div class="alert alert-warning alert-soft w-full text-left">
						<TriangleAlert class="size-5 shrink-0" />
						<span>This action may not be reversible.</span>
					</div>
				{/if}

				<div class="grid w-full grid-cols-2 gap-3 pt-2">
					<button class="btn btn-outline" onclick={no}>
						<X class="size-4" />
						{noText}
					</button>

					<button class={`btn ${warning ? 'btn-warning' : 'btn-primary'}`} onclick={yes}>
						<Check class="size-4" />
						{yesText}
					</button>
				</div>
			</div>
		</PopupCard>
	</BlurPage>
{/if}
