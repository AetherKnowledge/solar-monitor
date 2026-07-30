<script lang="ts">
	import { BookOpen, FunctionSquare, Hash, List, Plus, X } from '@lucide/svelte';

	import BlurPage from '$lib/popup/BlurPage.svelte';
	import { RegisterType } from '../DeviceTypes';

	type Props = {
		onCancel: () => void;
		onSelect: (type: RegisterType) => void;
	};

	const { onCancel, onSelect }: Props = $props();

	const registerTypes = [
		{
			type: RegisterType.Read,
			title: 'Read Register',
			description: 'Read values from the Modbus device.',
			icon: BookOpen,
			color: 'text-primary',
			bg: 'bg-primary/10'
		},
		{
			type: RegisterType.Virtual,
			title: 'Virtual Sensor',
			description: 'Create calculated sensors using expressions.',
			icon: FunctionSquare,
			color: 'text-secondary',
			bg: 'bg-secondary/10'
		},
		{
			type: RegisterType.Select,
			title: 'Select Register',
			description: 'Write predefined options to the device.',
			icon: List,
			color: 'text-accent',
			bg: 'bg-accent/10'
		},
		{
			type: RegisterType.Number,
			title: 'Number Register',
			description: 'Write numeric values to the device.',
			icon: Hash,
			color: 'text-warning',
			bg: 'bg-warning/10'
		}
	];
</script>

<BlurPage>
	<div
		class="animate-fade-up flex max-h-[calc(100dvh-2rem)] w-full max-w-3xl flex-col overflow-hidden rounded-2xl border border-base-300 bg-base-100 shadow-2xl"
	>
		<!-- Header -->
		<div class="shrink-0 border-b border-base-300 p-4 sm:p-6">
			<div class="flex items-start justify-between gap-3">
				<div class="flex items-start gap-3 sm:gap-4">
					<div class="rounded-2xl bg-primary/10 p-3 text-primary sm:p-4">
						<Plus class="size-6 sm:size-8" />
					</div>

					<div>
						<h2 class="text-lg font-semibold sm:text-xl">Add Register</h2>

						<p class="mt-1 text-xs text-base-content/60 sm:text-sm">
							Choose the type of register you would like to create.
						</p>
					</div>
				</div>

				<button class="btn btn-circle shrink-0 btn-ghost btn-sm" onclick={onCancel}>
					<X class="size-5" />
				</button>
			</div>
		</div>

		<!-- Body -->
		<div class="min-h-0 flex-1 overflow-y-auto p-4 sm:p-6">
			<div class="grid gap-4 md:grid-cols-2">
				{#each registerTypes as item (item.type)}
					<button
						class="card border border-base-300 bg-base-100 text-left transition-all duration-200 hover:border-primary hover:bg-base-200/40"
						onclick={() => onSelect(item.type)}
					>
						<div class="card-body gap-3 p-4">
							<div class={`${item.bg} ${item.color} w-fit rounded-xl p-3`}>
								<item.icon class="size-5 sm:size-6" />
							</div>

							<h3 class="text-base font-semibold sm:text-lg">
								{item.title}
							</h3>

							<p class="text-xs text-base-content/60 sm:text-sm">
								{item.description}
							</p>
						</div>
					</button>
				{/each}
			</div>
		</div>

		<!-- Footer -->
		<div class="shrink-0 border-t border-base-300 bg-base-200/30 p-4 sm:p-5">
			<button class="btn w-full btn-ghost sm:w-auto" onclick={onCancel}> Cancel </button>
		</div>
	</div>
</BlurPage>
