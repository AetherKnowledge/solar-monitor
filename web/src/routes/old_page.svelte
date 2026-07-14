<script lang="ts">
	import { fly } from 'svelte/transition';
	import Header from './old_header.svelte';

	let formState = $state({
		answers: {
			name: '',
			birthday: ''
		},
		step: 0,
		error: ''
	});

	const questions = [
		{ question: 'What is your name?', id: 'name', type: 'text' },
		{ question: 'What is your birthday?', id: 'birthday', type: 'date' }
	];

	function nextStep(id: string) {
		if (formState.answers[id]) {
			formState.step += 1;
			formState.error = '';
		} else {
			formState.error = 'Please fill out the field before proceeding.';
		}
	}

	$effect(() => {
		console.log('on mounted');

		return () => {
			console.log('on unmounted');
		};
	});

	$effect(() => {
		console.log('formState changed:', formState.step);

		return () => {
			console.log('cleanup for formState change', formState.step);
		};
	});
</script>

<main>
	<Header name={formState.answers.name || 'Guest'}>
		<h1>hello</h1>
	</Header>

	{#if formState.step === questions.length}
		<h2>Thank you, {formState.answers.name}!</h2>
	{:else}
		<p>Step: {formState.step + 1}</p>
	{/if}

	{#each questions as question, index (question.id)}
		{#if formState.step === index}
			<div in:fly={{ y: 20, duration: 300, opacity: 0 }}>
				{@render formStep(question)}
			</div>
		{/if}
	{/each}

	{#if formState.error}
		<p style="color: red">{formState.error}</p>
	{/if}
</main>
