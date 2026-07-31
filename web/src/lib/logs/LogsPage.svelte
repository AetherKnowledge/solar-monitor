<script lang="ts">
	import { Eraser, Logs, RefreshCw, Wifi, WifiOff } from '@lucide/svelte';
	import { onMount } from 'svelte';

	type LogEntry = {
		id: number;
		message: string;
		timestamp: Date;
	};

	let entries: LogEntry[] = $state([]);
	let connected = $state(false);
	let connecting = $state(false);
	let autoScroll = $state(true);

	let socket: WebSocket | null = null;
	let logContainer: HTMLDivElement;
	let reconnectTimer: ReturnType<typeof setTimeout> | null = null;
	let nextId = 0;

	const MAX_LOGS = 1000;

	function connect() {
		if (socket?.readyState === WebSocket.OPEN || socket?.readyState === WebSocket.CONNECTING) {
			return;
		}

		connecting = true;

		const protocol = location.protocol === 'https:' ? 'wss:' : 'ws:';
		const url = `${protocol}//${location.host}/api/ws`;

		socket = new WebSocket(url);

		socket.onopen = () => {
			connected = true;
			connecting = false;

			addSystemLog('Connected to device');
		};

		function updateLastLog(message: string) {
			if (entries.length === 0) {
				addLog(message);
				return;
			}

			entries[entries.length - 1] = {
				...entries[entries.length - 1],
				message,
				timestamp: new Date()
			};

			scrollToBottom();
		}

		socket.onmessage = (event) => {
			const message = String(event.data);

			if (message.endsWith('\r')) {
				updateLastLog(message.slice(0, -1));
			} else {
				addLog(message.replace(/\r?\n$/, ''));
			}
		};

		socket.onclose = () => {
			const wasConnected = connected;

			connected = false;
			connecting = false;
			socket = null;

			if (wasConnected) {
				addSystemLog('Connection lost');
			}

			scheduleReconnect();
		};

		socket.onerror = () => {
			socket?.close();
		};
	}

	function scheduleReconnect() {
		if (reconnectTimer) return;

		reconnectTimer = setTimeout(() => {
			reconnectTimer = null;
			connect();
		}, 2000);
	}

	function reconnect() {
		if (reconnectTimer) {
			clearTimeout(reconnectTimer);
			reconnectTimer = null;
		}

		socket?.close();
		socket = null;

		connect();
	}

	function addLog(message: string) {
		entries.push({
			id: nextId++,
			message,
			timestamp: new Date()
		});

		if (entries.length > MAX_LOGS) {
			entries.splice(0, entries.length - MAX_LOGS);
		}

		scrollToBottom();
	}

	function addSystemLog(message: string) {
		addLog(`[System] ${message}`);
	}

	function clearLogs() {
		entries = [];
	}

	function scrollToBottom() {
		if (!autoScroll) return;

		requestAnimationFrame(() => {
			if (logContainer) {
				logContainer.scrollTop = logContainer.scrollHeight;
			}
		});
	}

	function formatTime(date: Date) {
		return date.toLocaleTimeString([], {
			hour: '2-digit',
			minute: '2-digit',
			second: '2-digit',
			hour12: false
		});
	}

	onMount(() => {
		connect();

		return () => {
			if (reconnectTimer) {
				clearTimeout(reconnectTimer);
			}

			socket?.close();
		};
	});
</script>

<div class="flex h-full min-h-0 w-full max-w-7xl flex-col">
	<div class="card flex h-full min-h-0 flex-1 border border-base-300 bg-base-100 shadow-sm">
		<div class="card-body flex min-h-0 flex-1 flex-col gap-4 sm:gap-6">
			<!-- Header -->
			<div class="flex flex-col gap-4 sm:flex-row sm:items-center sm:justify-between">
				<div class="flex items-center gap-3">
					<div class="rounded-xl bg-primary/10 p-2.5 text-primary sm:p-3">
						<Logs class="size-6 sm:size-7" />
					</div>

					<div>
						<h2 class="card-title text-lg sm:text-xl">System Logs</h2>

						<p class="text-xs text-base-content/60 sm:text-sm">
							Live device output and system messages.
						</p>
					</div>
				</div>

				<div class="self-start sm:self-auto">
					{#if connected}
						<div class="badge gap-1.5 text-xs badge-success sm:text-sm">
							<Wifi class="size-3 sm:size-3.5" />
							Connected
						</div>
					{:else if connecting}
						<div class="badge gap-1.5 text-xs badge-warning sm:text-sm">
							<span class="loading loading-xs loading-spinner"></span>
							Connecting
						</div>
					{:else}
						<div class="badge gap-1.5 text-xs badge-error sm:text-sm">
							<WifiOff class="size-3 sm:size-3.5" />
							Disconnected
						</div>
					{/if}
				</div>
			</div>

			<div class="divider my-0"></div>

			<!-- Controls -->
			<div class="flex flex-col gap-4 sm:flex-row sm:items-center sm:justify-between">
				<div>
					<h3 class="font-semibold">Live Output</h3>
					<p class="text-sm text-base-content/60">Real-time log messages from the device.</p>
				</div>

				<div class="flex flex-wrap items-center gap-2 sm:justify-end">
					<label class="flex cursor-pointer items-center gap-2">
						<span class="text-xs text-base-content/60 sm:text-sm">Auto scroll</span>

						<input
							type="checkbox"
							class="toggle toggle-primary toggle-sm sm:toggle-xs"
							bind:checked={autoScroll}
							onchange={() => {
								if (autoScroll) {
									scrollToBottom();
								}
							}}
						/>
					</label>

					<div class="divider mx-0 hidden divider-horizontal sm:flex"></div>

					<button class="btn btn-ghost btn-sm sm:btn-xs" onclick={reconnect} disabled={connecting}>
						{#if connecting}
							<span class="loading loading-xs loading-spinner"></span>
						{:else}
							<RefreshCw class="size-3.5 sm:size-4" />
						{/if}

						Reconnect
					</button>

					<button
						class="btn btn-ghost btn-sm sm:btn-xs"
						onclick={clearLogs}
						disabled={entries.length === 0}
					>
						<Eraser class="size-3.5 sm:size-4" />
						Clear
					</button>
				</div>
			</div>

			<!-- Terminal -->
			<div bind:this={logContainer} class="mockup-code min-h-0 flex-1 overflow-auto">
				{#if entries.length === 0}
					<pre data-prefix=">" class="text-base-content/50"><code>Waiting for device logs...</code
						></pre>
				{:else}
					{#each entries as entry, index (entry.id)}
						<pre
							data-prefix={index + 1}
							class:text-info={entry.message.startsWith('[System]')}><code
								>{formatTime(entry.timestamp)} {entry.message}</code
							></pre>
					{/each}
				{/if}
			</div>

			<!-- Footer -->
			<div class="flex items-center justify-between text-[10px] text-base-content/50 sm:text-xs">
				<div class="flex items-center gap-2">
					<span class="size-2 rounded-full" class:bg-success={connected} class:bg-error={!connected}
					></span>

					{#if connected}
						Receiving live logs
					{:else if connecting}
						Connecting to device...
					{:else}
						Disconnected
					{/if}
				</div>

				<span class="font-mono">
					{entries.length} / {MAX_LOGS}
				</span>
			</div>
		</div>
	</div>
</div>
