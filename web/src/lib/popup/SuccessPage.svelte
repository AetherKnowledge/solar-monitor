<script lang="ts">
	import { CircleCheck, X } from '@lucide/svelte';
	import BlurPage from './BlurPage.svelte';
	import PopupCard from './PopupCard.svelte';

	type Props = {
		title?: string;
		message?: string;
		buttonText?: string;
		onClose?: () => void;
	};

	let {
		title = 'Success!',
		message = 'The operation completed successfully.',
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
		<PopupCard>
			<div class="card-body items-center text-center gap-6">
				<div
					class="flex size-24 items-center justify-center rounded-full bg-success/15 text-success"
				>
					<CircleCheck class="size-14" />
				</div>

				<div class="space-y-2">
					<h2 class="text-2xl font-bold">{title}</h2>

					<p class="text-base-content/70">
						{message}
					</p>
				</div>

				<div class="w-full pt-2">
					<button class="btn btn-success w-full" onclick={close}>
						<X class="size-4" />
						{buttonText}
					</button>
				</div>
			</div>
		</PopupCard>
	</BlurPage>
{/if}
