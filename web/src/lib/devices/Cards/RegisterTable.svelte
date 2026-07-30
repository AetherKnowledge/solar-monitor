<script lang="ts">
	import { Plus, Search, TableProperties } from '@lucide/svelte';
	import { RegisterType, type RegisterListItem } from '../DeviceTypes';
	import RegisterCard from './RegisterCard.svelte';
	import RegisterTableRow from './RegisterTableRow.svelte';

	type Props = {
		registers: RegisterListItem[];
		onAddRegister?: () => void;
		onEdit?: (register: RegisterListItem) => void;
	};

	let { registers, onEdit, onAddRegister }: Props = $props();

	let searchQuery: string = $state('');
	let selectedType: RegisterType | 'All Types' = $state('All Types');

	let filteredRegisters: RegisterListItem[] = $derived.by(() => {
		return registers.filter((register) => {
			const matchesSearch =
				register.name.toLowerCase().includes(searchQuery.toLowerCase()) ||
				(register.address !== undefined && register.address.toString().includes(searchQuery)) ||
				(register.unit && register.unit.toLowerCase().includes(searchQuery.toLowerCase()));
			const matchesType = selectedType === 'All Types' || register.type === selectedType;
			return matchesSearch && matchesType;
		});
	});
</script>

<div class="card border border-base-300 bg-base-100 shadow-sm">
	<div class="card-body">
		<!-- Header -->
		<div class="flex flex-col gap-4 pb-3 md:flex-row md:items-center md:justify-between">
			<div class="flex items-center gap-3">
				<div class="rounded-xl bg-primary/10 p-3 text-primary">
					<TableProperties class="size-6" />
				</div>

				<div>
					<h2 class="card-title">Registers</h2>
					<p class="text-sm text-base-content/60">
						View, search, and configure Modbus registers for this device.
					</p>
				</div>
			</div>

			<button class="btn btn-primary" onclick={onAddRegister}>
				<Plus class="size-4" />
				Add Register
			</button>
		</div>

		<!-- Toolbar -->
		<div class="flex flex-col gap-3 md:flex-row md:items-center">
			<label class="input-bordered input w-full md:flex-1">
				<Search class="size-4 opacity-60" />
				<input
					type="search"
					class="w-full"
					placeholder="Search by name, address, or unit..."
					bind:value={searchQuery}
				/>
			</label>

			<select class="select-bordered select w-full md:w-40" bind:value={selectedType}>
				<option>All Types</option>
				<option>Read</option>
				<option>Virtual</option>
				<option>Select</option>
				<option>Number</option>
			</select>
		</div>

		<!-- Mobile -->
		<div class="space-y-3 md:hidden">
			{#if filteredRegisters.length === 0}
				<div class="rounded-box border border-base-300 p-8 text-center text-base-content/60">
					No registers found.
				</div>
			{:else}
				{#each filteredRegisters as register, index (index)}
					<RegisterCard {register} onEdit={(r) => onEdit?.(r)} />
				{/each}
			{/if}
		</div>

		<!-- Desktop -->
		<div class="hidden md:block">
			<div class="h-128 overflow-auto rounded-box border border-base-300">
				<table class="table table-zebra table-sm">
					<thead class="sticky top-0 z-10 bg-base-100">
						<tr>
							<th>Name</th>
							<th class="text-center">Type</th>
							<th class="text-center">Address</th>
							<th class="text-center">Value</th>
							<th class="text-center">Unit</th>
							<th class="w-40 pr-6 text-right">Actions</th>
						</tr>
					</thead>

					<tbody>
						{#each filteredRegisters as register, index (index)}
							<RegisterTableRow {register} onEdit={(r) => onEdit?.(r)} />
						{/each}
					</tbody>
				</table>
			</div>
		</div>
	</div>
</div>
