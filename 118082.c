set_wc_option_mod_status(optmask, status)
unsigned long optmask;
int status;
{
	int k = 0;
	if ( SET__IS_VALUE_VALID(status) ) {
		impossible("set_wc_option_mod_status: status out of range %d.",
			   status);
		return;
	}
	while (wc_options[k].wc_name) {
		if (optmask & wc_options[k].wc_bit) {
			set_option_mod_status(wc_options[k].wc_name, status);
		}
		k++;
	}
}