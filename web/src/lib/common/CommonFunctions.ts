export async function apiFetch<T>(url: string, init?: RequestInit): Promise<T> {
	const response = await fetch(url, init);

	if (!response.ok) {
		let message = `${response.status} ${response.statusText}`;

		try {
			const body = await response.json();
			if (body.message) {
				message = body.message;
			}
		} catch {
			// Ignore if the response wasn't JSON
		}

		throw new Error(message);
	}

	return response.json() as Promise<T>;
}
