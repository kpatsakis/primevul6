static int parse_pack_def(const char *line, int old_cnt)
{
	struct pack_info *i = find_pack_by_name(line + 2);
	if (i) {
		i->old_num = old_cnt;
		return 0;
	}
	else {
		/* The file describes a pack that is no longer here */
		return 1;
	}
}