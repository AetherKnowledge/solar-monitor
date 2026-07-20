export type WithUpdateStatus<T> = T & {
	updateStatus: UpdateStatus;
};

export enum UpdateStatus {
	Requested = 'requested',
	InProgress = 'in_progress',
	UpdateComplete = 'complete',
	UpdateFailed = 'failed',
	NotStarted = 'not_started'
}

export type SimpleResponse = {
	message: string;
};
