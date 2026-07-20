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
	<div class="bg-base-100 shadow-2xl border border-base-300 animate-fade-up w-[80%]">
		<div class="flex max-h-[80vh] flex-col w-full">
			<!-- Header -->
			<div class="border-b border-base-300 p-6">
				<div class="flex items-start justify-between gap-4">
					<div class="flex items-center gap-4">
						<div class={`${info.bg} ${info.color} rounded-2xl p-4`}>
							<Icon class="size-8" />
						</div>

						<div>
							<h2 class="text-xl font-semibold">
								Edit {info.title}
							</h2>

							<p class="text-base-content/60 mt-1 max-w-lg text-sm">
								{info.description}
							</p>
						</div>
					</div>

					<button class="btn btn-circle btn-ghost btn-sm" onclick={onCancel}>
						<X class="size-5" />
					</button>
				</div>
			</div>

			<!-- Body -->
			{#if register.type === RegisterType.Read}
				<ReadRegisterBody bind:register={register.register} />
			{:else if register.type === RegisterType.Virtual}
				<VirtualSensorBody bind:register={register.register} />
			{:else if register.type === RegisterType.Number}
				<NumberWriteBody bind:register={register.register} />
			{:else if register.type === RegisterType.Select}
				<SelectWriteBody bind:register={register.register} />
			{/if}

			<!-- Footer -->
			<div class="border-t border-base-300 bg-base-200/30 flex items-center justify-between p-5">
				{#if !isNew}
					<button class="btn btn-error btn-soft" onclick={() => onDelete?.(register)}>
						<Trash2 class="size-4" />
						Delete
					</button>
				{/if}

				<div class="flex gap-2">
					<button class="btn btn-ghost" onclick={onCancel}> Cancel </button>

					<button class="btn btn-primary" onclick={onSave}> Save Changes </button>
				</div>
			</div>
		</div>
	</div>
</BlurPage>
