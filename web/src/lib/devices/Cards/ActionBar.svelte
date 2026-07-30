<script lang="ts">
	type Props = {
		hasChanged: boolean;
		isSaving?: boolean;
		onSave: () => void;
		onCancel: () => void;
	};

	const { hasChanged, isSaving = false, onSave, onCancel }: Props = $props();
</script>

<div
	class="sticky bottom-0 z-10 rounded-2xl border border-base-300 bg-base-100/90 shadow-xl backdrop-blur"
>
	<div class="flex flex-col gap-4 p-4 lg:flex-row lg:items-center lg:justify-between">
		<div class="flex flex-col gap-2">
			{#if hasChanged}
				<div class="badge w-fit rounded-lg badge-sm badge-warning sm:badge-md">Unsaved Changes</div>

				<p class="text-xs text-base-content/70 sm:text-sm">
					You have pending changes that haven't been saved.
				</p>
			{:else}
				<div class="badge w-fit rounded-lg badge-sm badge-success sm:badge-md">
					Everything Saved
				</div>

				<p class="text-xs text-base-content/70 sm:text-sm">All changes have been saved.</p>
			{/if}
		</div>

		<div class="flex w-full gap-2 lg:w-auto">
			<button
				type="button"
				class="btn flex-1 btn-outline btn-sm lg:flex-none"
				disabled={!hasChanged || isSaving}
				onclick={onCancel}
			>
				Cancel
			</button>

			<button
				type="button"
				class="btn flex-1 btn-primary btn-sm lg:min-w-32 lg:flex-none"
				disabled={!hasChanged || isSaving}
				onclick={onSave}
			>
				{#if isSaving}
					<span class="loading loading-xs loading-spinner"></span>
					Saving...
				{:else}
					Save Changes
				{/if}
			</button>
		</div>
	</div>
</div>
