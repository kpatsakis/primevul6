get_fkey_list(TERMTYPE2 *tp)
{
    NAME_VALUE *result = typeMalloc(NAME_VALUE, NUM_STRINGS(tp) + 1);
    const struct tinfo_fkeys *all_fkeys = _nc_tinfo_fkeys;
    int used = 0;
    unsigned j;

    if (result == 0)
	failed("get_fkey_list");

    for (j = 0; all_fkeys[j].code; j++) {
	char *a = tp->Strings[all_fkeys[j].offset];
	if (VALID_STRING(a)) {
	    result[used].keycode = (int) all_fkeys[j].code;
	    result[used].name = strnames[all_fkeys[j].offset];
	    result[used].value = a;
	    ++used;
	}
    }
#if NCURSES_XNAMES
    for (j = STRCOUNT; j < NUM_STRINGS(tp); ++j) {
	const char *name = ExtStrname(tp, (int) j, strnames);
	if (*name == 'k') {
	    result[used].keycode = -1;
	    result[used].name = name;
	    result[used].value = tp->Strings[j];
	    ++used;
	}
    }
#endif
    result[used].keycode = 0;
    return result;
}