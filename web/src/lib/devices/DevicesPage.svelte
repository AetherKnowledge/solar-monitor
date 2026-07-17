<script lang="ts">
	import DeviceCard from '$lib/devices/DeviceCard.svelte';
	import { createDevicesConfigQueryOptions } from '$lib/devices/DevicesQueries';
	import { createQuery } from '@tanstack/svelte-query';
	import type { ModbusDevice } from './DeviceTypes';

	let devicesQuery = createQuery<ModbusDevice[]>(() => createDevicesConfigQueryOptions());
	let devices = $derived(devicesQuery.data || []);
</script>

{#each devices as device (device.identifier)}
	<DeviceCard {device} />
{/each}
