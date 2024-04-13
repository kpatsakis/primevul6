static size_t ea_list_size(struct ea_list *ealist)
{
	fstring dos_ea_name;
	struct ea_list *listp;
	size_t ret = 0;

	for (listp = ealist; listp; listp = listp->next) {
		push_ascii_fstring(dos_ea_name, listp->ea.name);
		ret += 4 + strlen(dos_ea_name) + 1 + listp->ea.value.length;
	}
	/* Add on 4 for total length. */
	if (ret) {
		ret += 4;
	}

	return ret;
}