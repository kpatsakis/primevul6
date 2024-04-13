drv_keyok(TERMINAL_CONTROL_BLOCK * TCB, int c, int flag)
{
    SCREEN *sp;
    int code = ERR;
    int count = 0;
    char *s;

    AssertTCB();
    SetSP();

    if (c >= 0) {
	unsigned ch = (unsigned) c;
	if (flag) {
	    while ((s = _nc_expand_try(sp->_key_ok,
				       ch, &count, (size_t) 0)) != 0) {
		if (_nc_remove_key(&(sp->_key_ok), ch)) {
		    code = _nc_add_to_try(&(sp->_keytry), s, ch);
		    free(s);
		    count = 0;
		    if (code != OK)
			break;
		} else {
		    free(s);
		}
	    }
	} else {
	    while ((s = _nc_expand_try(sp->_keytry,
				       ch, &count, (size_t) 0)) != 0) {
		if (_nc_remove_key(&(sp->_keytry), ch)) {
		    code = _nc_add_to_try(&(sp->_key_ok), s, ch);
		    free(s);
		    count = 0;
		    if (code != OK)
			break;
		} else {
		    free(s);
		}
	    }
	}
    }
    return (code);
}