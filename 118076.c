set_option_mod_status(optnam, status)
const char *optnam;
int status;
{
	int k;
	if ( SET__IS_VALUE_VALID(status) ) {
		impossible("set_option_mod_status: status out of range %d.",
			   status);
		return;
	}
	for (k = 0; boolopt[k].name; k++) {
		if (!strncmpi(boolopt[k].name, optnam, strlen(optnam))) {
			boolopt[k].optflags = status;
			return;
		}
	}
	for (k = 0; compopt[k].name; k++) {
		if (!strncmpi(compopt[k].name, optnam, strlen(optnam))) {
			compopt[k].optflags = status;
			return;
		}
	}
}