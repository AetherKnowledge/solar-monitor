<script lang="ts">
	import ScrollPage from '$lib/common/ScrollPage.svelte';
	import Dashboard from '$lib/dashboard/Dashboard.svelte';
</script>

<ScrollPage>
	<Dashboard />

	<!-- weird bug code mockup not appearing correctly in the logs page without this -->
	{#if false}
		<div class="mockup-code w-full">
			<pre data-prefix="$"><code>npm i daisyui</code></pre>
			<pre data-prefix=">" class="text-warning"><code>installing...</code></pre>
			<pre data-prefix=">" class="text-success"><code>Done!</code></pre>
		</div>
	{/if}
</ScrollPage>
