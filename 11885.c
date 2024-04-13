_nc_use_tracef(unsigned mask)
{
    bool result = FALSE;

    _nc_lock_global(tst_tracef);
    if (!MyNested++) {
	if ((result = (_nc_tracing & (mask))) != 0
	    && _nc_try_global(tracef) == 0) {
	    /* we will call _nc_locked_tracef(), no nesting so far */
	} else {
	    /* we will not call _nc_locked_tracef() */
	    MyNested = 0;
	}
    } else {
	/* we may call _nc_locked_tracef(), but with nested_tracef > 0 */
	result = (_nc_tracing & (mask));
    }
    _nc_unlock_global(tst_tracef);
    return result;
}