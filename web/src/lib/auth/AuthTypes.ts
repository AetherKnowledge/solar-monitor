export interface SiteConfig {
	passwordEnabled: boolean;
	password: string;
	hasPassword: boolean;
}

export const defaultSiteConfig: SiteConfig = {
	passwordEnabled: false,
	password: '',
	hasPassword: false
};
