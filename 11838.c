getparm(int parm, int n)
/* push n copies of param on the terminfo stack if not already there */
{
    int nn;

    if (seenr) {
	if (parm == 1)
	    parm = 2;
	else if (parm == 2)
	    parm = 1;
    }

    for (nn = 0; nn < n; ++nn) {
	dp = save_string(dp, "%p");
	dp = save_char(dp, '0' + parm);
    }

    if (onstack == parm) {
	if (n > 1) {
	    _nc_warning("string may not be optimal");
	    dp = save_string(dp, "%Pa");
	    while (n-- > 0) {
		dp = save_string(dp, "%ga");
	    }
	}
	return;
    }
    if (onstack != 0)
	push();

    onstack = parm;

    if (seenn && parm < 3) {
	dp = save_string(dp, "%{96}%^");
    }

    if (seenm && parm < 3) {
	dp = save_string(dp, "%{127}%^");
    }
}