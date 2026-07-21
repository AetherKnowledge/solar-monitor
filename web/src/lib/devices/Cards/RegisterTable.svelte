<script lang="ts">
	import { Plus, Search, TableProperties } from '@lucide/svelte';
	import { RegisterType, type RegisterListItem } from '../DeviceTypes';
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
			<label class="input-bordered input flex-1">
				<Search class="size-4 opacity-60" />
				<input
					type="search"
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

		<!-- Scrollable Table -->
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
					{#if filteredRegisters.length === 0}
						<tr>
							<td colspan="6" class="h-48 text-center text-base-content/60">
								No registers found.
							</td>
						</tr>
					{:else}
						{#each filteredRegisters as register, index (index)}
							<RegisterTableRow {register} onEdit={(r) => onEdit?.(r)} />
						{/each}
					{/if}
				</tbody>
			</table>
		</div>
	</div>
</div>
