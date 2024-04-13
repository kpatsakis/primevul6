increment(int *all_parms, int *num_parms, int len_parms, int end_parms)
{
    int rc = 0;
    int n;

    if (len_parms > 9)
	len_parms = 9;

    if (end_parms < len_parms) {
	if (all_parms[end_parms]++ >= num_parms[end_parms]) {
	    all_parms[end_parms] = 0;
	    increment(all_parms, num_parms, len_parms, end_parms + 1);
	}
    }
    for (n = 0; n < len_parms; ++n) {
	if (all_parms[n] != 0) {
	    rc = 1;
	    break;
	}
    }
    /* return 1 until the vector resets to all 0's */
    return rc;
}