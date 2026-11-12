export async function apiFetch<T>(
	url: string,
	init?: RequestInit,
	redirectUnauthorized: boolean = true
): Promise<T> {
	const response = await fetch(url, init);

	if (redirectUnauthorized && response.status === 401) {
		window.location.href = '/login';
		throw new Error('Unauthorized');
	}

	if (!response.ok) {
		let message = `${response.status} ${response.statusText}`;

		try {
			const body = await response.json();

			if (body.message) {
				message = body.message;
			}
		} catch {
			// Response wasn't JSON
		}

		throw new Error(message);
	}

	return response.json() as Promise<T>;
}
