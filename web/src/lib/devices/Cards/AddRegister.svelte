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
		class="bg-base-100 animate-fade-up w-full max-w-3xl rounded-2xl border border-base-300 shadow-2xl"
	>
		<!-- Header -->
		<div class="border-b border-base-300 p-6">
			<div class="flex items-start justify-between">
				<div class="flex items-center gap-4">
					<div class="bg-primary/10 text-primary rounded-2xl p-4">
						<Plus class="size-8" />
					</div>

					<div>
						<h2 class="text-xl font-semibold">Add Register</h2>

						<p class="text-base-content/60 mt-1 text-sm">
							Choose the type of register you would like to create.
						</p>
					</div>
				</div>

				<button class="btn btn-circle btn-ghost btn-sm" onclick={onCancel}>
					<X class="size-5" />
				</button>
			</div>
		</div>

		<!-- Body -->
		<div class="grid gap-5 p-6 md:grid-cols-2">
			{#each registerTypes as item (item.type)}
				<button
					class="card bg-base-100 hover:border-primary hover:bg-base-200/40 border border-base-300 text-left transition-all duration-200"
					onclick={() => onSelect(item.type)}
				>
					<div class="card-body">
						<div class={`${item.bg} ${item.color} mb-2 w-fit rounded-xl p-3`}>
							<item.icon class="size-6" />
						</div>

						<h3 class="text-lg font-semibold">
							{item.title}
						</h3>

						<p class="text-base-content/60 text-sm">
							{item.description}
						</p>
					</div>
				</button>
			{/each}
		</div>

		<!-- Footer -->
		<div class="border-t border-base-300 bg-base-200/30 flex justify-end p-5">
			<button class="btn btn-ghost" onclick={onCancel}> Cancel </button>
		</div>
	</div>
</BlurPage>
