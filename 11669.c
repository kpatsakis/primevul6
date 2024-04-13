is_user_capability(const char *name)
{
    int result = -1;
    if (name[0] == 'u' &&
	(name[1] >= '0' && name[1] <= '9') &&
	name[2] == '\0') {
	result = (name[1] == '6') ? 2 : 0;
    }
#if NCURSES_XNAMES
    else if (using_extensions) {
	struct user_table_entry const *p = lookup_user_capability(name);
	if (p != 0) {
	    result = (int) p->ute_argc;
	}
    }
#endif
    return result;
}