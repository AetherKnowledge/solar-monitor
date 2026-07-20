<script lang="ts">
	import DeviceCard from '$lib/devices/DeviceCard.svelte';
	import { createDevicesController } from './DeviceController.svelte';
	const { query } = createDevicesController();
	const devices = $derived(query.data?.devices || []);
</script>

{#each devices as device (device.identifier)}
	<DeviceCard {device} />
{/each}
