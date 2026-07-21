<script lang="ts">
	import { hidePopup, popup, PopupType } from './Popup.svelte';

	import ErrorPage from './ErrorPage.svelte';
	import LoadingPage from './LoadingPage.svelte';
	import SuccessPage from './SuccessPage.svelte';
	import YesNoPage from './YesNoPage.svelte';
</script>

{#if popup.type === PopupType.LOADING}
	<LoadingPage text={popup.message} progress={popup.progress} />
{/if}

{#if popup.type === PopupType.SUCCESS}
	<SuccessPage
		message={popup.message}
		onClose={() => {
			const callback = popup.onClose;
			hidePopup();
			callback?.();
		}}
	/>
{/if}

{#if popup.type === PopupType.ERROR}
	<ErrorPage
		message={popup.message}
		onClose={() => {
			const callback = popup.onClose;
			hidePopup();
			callback?.();
		}}
	/>
{/if}

{#if popup.type === PopupType.YESNO}
	<YesNoPage
		message={popup.message}
		onYes={() => {
			popup.resolve?.(true);
			hidePopup();
		}}
		onNo={() => {
			popup.resolve?.(false);
			hidePopup();
		}}
	/>
{/if}
