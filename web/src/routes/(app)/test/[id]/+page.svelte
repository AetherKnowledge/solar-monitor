<script lang="ts">
	import { page } from '$app/state';

	let id: string | undefined = page.params.id;
	let count = $state(1)

	function add(){
		count = count + 1;
	}
</script>

<h1>
	Hello from the test page with id: {id}
</h1>
<button class="btn btn-success" onclick={add}>{count}</button>