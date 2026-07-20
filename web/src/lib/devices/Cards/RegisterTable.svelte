<script lang="ts">
	import { Plus, Search, TableProperties } from '@lucide/svelte';
	import { RegisterType, type RegisterListItem } from '../DeviceTypes';
	import RegisterTableRow from './RegisterTableRow.svelte';

	type Props = {
		registers: RegisterListItem[];
		onEdit?: (register: RegisterListItem) => void;
	};

	let { registers, onEdit }: Props = $props();

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

<div class="card bg-base-100 border border-base-300 shadow-sm">
	<div class="card-body">
		<!-- Header -->
		<div class="flex flex-col gap-4 pb-3 md:flex-row md:items-center md:justify-between">
			<div class="flex items-center gap-3">
				<div class="bg-primary/10 text-primary rounded-xl p-3">
					<TableProperties class="size-6" />
				</div>

				<div>
					<h2 class="card-title">Registers</h2>
					<p class="text-base-content/60 text-sm">
						View, search, and configure Modbus registers for this device.
					</p>
				</div>
			</div>

			<button class="btn btn-primary">
				<Plus class="size-4" />
				Add Register
			</button>
		</div>

		<!-- Toolbar -->
		<div class="flex flex-col gap-3 md:flex-row md:items-center">
			<label class="input input-bordered flex-1">
				<Search class="size-4 opacity-60" />
				<input
					type="search"
					placeholder="Search by name, address, or unit..."
					bind:value={searchQuery}
				/>
			</label>

			<select class="select select-bordered w-full md:w-40" bind:value={selectedType}>
				<option>All Types</option>
				<option>Read</option>
				<option>Virtual</option>
				<option>Select</option>
				<option>Number</option>
			</select>
		</div>

		<!-- Scrollable Table -->
		<div class="overflow-auto h-128 rounded-box border border-base-300">
			<table class="table table-zebra table-sm">
				<thead class="sticky top-0 bg-base-100 z-10">
					<tr>
						<th>Name</th>
						<th class="text-center">Type</th>
						<th class="text-center">Address</th>
						<th class="text-center">Value</th>
						<th class="text-center">Unit</th>
						<th class="w-40 text-right pr-6">Actions</th>
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
						{#each filteredRegisters as register (register.id)}
							<RegisterTableRow {register} onEdit={(r) => onEdit?.(r)} />
						{/each}
					{/if}
				</tbody>
			</table>
		</div>
	</div>
</div>
