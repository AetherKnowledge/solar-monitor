export enum PopupType {
	NONE,
	LOADING,
	SUCCESS,
	ERROR,
	YESNO
}

export type PopupState = {
	type: PopupType;
	title?: string;
	message?: string;
	progress?: number | null;
	onClose?: () => void;
	resolve?: (value: boolean) => void;
};

export const popup = $state<PopupState>({
	type: PopupType.NONE
});

export function hidePopup() {
	popup.type = PopupType.NONE;
	popup.title = undefined;
	popup.message = undefined;
	popup.progress = undefined;
	popup.onClose = undefined;
}

export function showLoading(message = 'Loading...') {
	popup.type = PopupType.LOADING;
	popup.message = message;
}

export function showSuccess(message: string, onClose?: () => void) {
	popup.type = PopupType.SUCCESS;
	popup.message = message;
	popup.onClose = onClose;
}

export function showError(message: string, onClose?: () => void) {
	popup.type = PopupType.ERROR;
	popup.message = message;
	popup.onClose = onClose;
}

export function showYesNo(message: string): Promise<boolean> {
	popup.type = PopupType.YESNO;
	popup.message = message;

	return new Promise((resolve) => {
		popup.resolve = resolve;
	});
}
