<script lang="ts">
	import { TriangleAlert, X } from '@lucide/svelte';
	import BlurPage from './BlurPage.svelte';

	type Props = {
		title?: string;
		message?: string;
		buttonText?: string;
		onClose?: () => void;
	};

	let {
		title = 'Something went wrong',
		message = 'An unexpected error occurred. Please try again.',
		buttonText = 'Close',
		onClose
	}: Props = $props();

	let visible = $state(true);

	function close() {
		visible = false;
		onClose?.();
	}
</script>

{#if visible}
	<BlurPage>
		<div class="card-body items-center text-center gap-6">
			<div class="flex size-24 items-center justify-center rounded-full bg-error/15 text-error">
				<TriangleAlert class="size-14" />
			</div>

			<div class="space-y-2">
				<h2 class="text-2xl font-bold">{title}</h2>

				<p class="text-base-content/70 wrap-break-words">
					{message}
				</p>
			</div>

			<div class="alert alert-error alert-soft w-full text-left">
				<TriangleAlert class="size-5 shrink-0" />
				<span>If the problem persists, please check your connection or try again later.</span>
			</div>

			<div class="w-full pt-2">
				<button class="btn btn-error w-full" onclick={close}>
					<X class="size-4" />
					{buttonText}
				</button>
			</div>
		</div>
	</BlurPage>
{/if}
