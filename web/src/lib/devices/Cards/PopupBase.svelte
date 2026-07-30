<script lang="ts">
	import { BookOpen, Hash, List, SquareFunction } from '@lucide/svelte';

	import { RegisterType, type RegisterItem } from '$lib/devices/DeviceTypes';
	import BlurPage from '$lib/popup/BlurPage.svelte';
	import { Trash2, X } from '@lucide/svelte';
	import NumberWriteBody from './NumberWriteBody.svelte';
	import ReadRegisterBody from './ReadRegisterBody.svelte';
	import SelectWriteBody from './SelectWriteBody.svelte';
	import VirtualSensorBody from './VirtualSensorBody.svelte';

	type Props = {
		register: RegisterItem;
		onSave?: () => void;
		onCancel?: () => void;
		onDelete?: (register: RegisterItem) => void;
		isNew?: boolean;
	};

	const { register = $bindable(), onSave, onCancel, onDelete, isNew = false }: Props = $props();

	const info = $derived.by(() => {
		switch (register.type) {
			case RegisterType.Read:
				return {
					title: 'Read Register',
					description: 'Read values from your Modbus device and expose them as sensors.',
					icon: BookOpen,
					color: 'text-primary',
					bg: 'bg-primary/10'
				};

			case RegisterType.Virtual:
				return {
					title: 'Virtual Sensor',
					description: 'Create calculated sensors using expressions based on other registers.',
					icon: SquareFunction,
					color: 'text-secondary',
					bg: 'bg-secondary/10'
				};

			case RegisterType.Select:
				return {
					title: 'Select Register',
					description: 'Write predefined options to a Modbus register using a dropdown.',
					icon: List,
					color: 'text-accent',
					bg: 'bg-accent/10'
				};

			case RegisterType.Number:
				return {
					title: 'Number Register',
					description: 'Write numeric values to a Modbus register with configurable limits.',
					icon: Hash,
					color: 'text-warning',
					bg: 'bg-warning/10'
				};
		}
	});

	const Icon = $derived(info.icon);
</script>

<BlurPage>
	<div
		class="animate-fade-up flex max-h-[calc(100dvh-2rem)] w-full max-w-5xl flex-col overflow-hidden rounded-2xl border border-base-300 bg-base-100 shadow-2xl"
	>
		<!-- Header -->
		<div class="shrink-0 border-b border-base-300 p-4 sm:p-6">
			<div class="flex items-start justify-between gap-3 sm:gap-4">
				<div class="flex items-start gap-3 sm:gap-4">
					<div class={`${info.bg} ${info.color} rounded-2xl p-3 sm:p-4`}>
						<Icon class="size-6 sm:size-8" />
					</div>

					<div>
						<h2 class="text-lg font-semibold sm:text-xl">
							Edit {info.title}
						</h2>

						<p class="mt-1 max-w-lg text-xs text-base-content/60 sm:text-sm">
							{info.description}
						</p>
					</div>
				</div>

				<button class="btn btn-circle shrink-0 btn-ghost btn-sm" onclick={onCancel}>
					<X class="size-5" />
				</button>
			</div>
		</div>

		<!-- Body -->
		<div class="min-h-0 flex-1 overflow-y-auto">
			{#if register.type === RegisterType.Read}
				<ReadRegisterBody bind:register={register.register} />
			{:else if register.type === RegisterType.Virtual}
				<VirtualSensorBody bind:register={register.register} />
			{:else if register.type === RegisterType.Number}
				<NumberWriteBody bind:register={register.register} />
			{:else if register.type === RegisterType.Select}
				<SelectWriteBody bind:register={register.register} />
			{/if}
		</div>

		<!-- Footer -->
		<div class="shrink-0 border-t border-base-300 bg-base-200/30 p-4 sm:p-5">
			<div class="flex flex-col gap-2 sm:flex-row sm:items-center sm:justify-between">
				{#if !isNew}
					<button
						class="btn w-full btn-soft btn-error sm:w-auto"
						onclick={() => onDelete?.(register)}
					>
						<Trash2 class="size-4" />
						Delete
					</button>
				{:else}
					<div class="hidden sm:block"></div>
				{/if}

				<div class="flex gap-2">
					<button class="btn flex-1 btn-ghost sm:flex-none" onclick={onCancel}> Cancel </button>

					<button class="btn flex-1 btn-primary sm:flex-none" onclick={onSave}>
						Save Changes
					</button>
				</div>
			</div>
		</div>
	</div>
</BlurPage>
