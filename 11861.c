init_xterm_mouse(SCREEN *sp)
{
    sp->_mouse_type = M_XTERM;
    sp->_mouse_format = MF_X10;
    sp->_mouse_xtermcap = tigetstr("XM");
    if (VALID_STRING(sp->_mouse_xtermcap)) {
	char *code = strstr(sp->_mouse_xtermcap, "[?");
	if (code != 0) {
	    code += 2;
	    while ((*code >= '0') && (*code <= '9')) {
		char *next = code;
		while ((*next >= '0') && (*next <= '9')) {
		    ++next;
		}
		if (!strncmp(code, "1006", (size_t) (next - code))) {
		    sp->_mouse_format = MF_SGR1006;
		}
#ifdef EXP_XTERM_1005
		if (!strncmp(code, "1005", (size_t) (next - code))) {
		    sp->_mouse_format = MF_XTERM_1005;
		}
#endif
		if (*next == ';') {
		    while (*next == ';') {
			++next;
		    }
		    code = next;
		} else {
		    break;
		}
	    }
	}
    } else {
	int code = tigetnum("XM");
	switch (code) {
	case 1006:
	    break;
	default:
	    code = 1000;
	    break;
	}
	sp->_mouse_xtermcap = "\033[?1000%?%p1%{1}%=%th%el%;";
    }
}