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
	class="sticky bottom-4 z-10 mt-8 rounded-2xl border border-base-300 bg-base-100/90 shadow-xl backdrop-blur"
>
	<div class="flex flex-col gap-4 p-4 md:flex-row md:items-center md:justify-between">
		<div class="flex items-center gap-3">
			{#if hasChanged}
				<div class="badge badge-warning badge-lg rounded-xl w-40">Unsaved Changes</div>

				<p class="text-sm text-base-content/70">
					You have pending changes that haven't been saved.
				</p>
			{:else}
				<div class="badge badge-success badge-lg rounded-xl w-40">Everything Saved</div>

				<p class="text-sm text-base-content/70">All changes have been saved.</p>
			{/if}
		</div>

		<div class="flex gap-3">
			<button
				type="button"
				class="btn btn-outline"
				disabled={!hasChanged || isSaving}
				onclick={onCancel}
			>
				Cancel
			</button>

			<button
				type="button"
				class="btn btn-primary min-w-28"
				disabled={!hasChanged || isSaving}
				onclick={onSave}
			>
				{#if isSaving}
					<span class="loading loading-spinner loading-sm"></span>
					Saving...
				{:else}
					Save Changes
				{/if}
			</button>
		</div>
	</div>
</div>
