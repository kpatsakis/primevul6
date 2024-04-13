load_symset(s, which_set)
const char *s;
int which_set;
{
	clear_symsetentry(which_set, TRUE);

	symset[which_set].name = (char *)alloc(strlen(s)+1);
	Strcpy(symset[which_set].name,s);
	if (read_sym_file(which_set))
		switch_symbols(TRUE);
	else {
		clear_symsetentry(which_set, TRUE);
		return 0;
	}
	return 1;
}