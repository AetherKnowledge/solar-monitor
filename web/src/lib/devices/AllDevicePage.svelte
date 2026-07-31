<script lang="ts">
	import DeviceCard from '$lib/devices/DeviceCard.svelte';
	import { createDevicesController } from './DeviceController.svelte';
	const { query } = createDevicesController();
	const devices = $derived(query.data?.devices || []);

	$effect(() => {
		console.log('AllDevicePage devices data:', devices);
	});
</script>

<div class="flex flex-col gap-4">
	{#each devices as device (device.discovery.identifier)}
		<DeviceCard {device} />
	{/each}
</div>
