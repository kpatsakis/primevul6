static void init_pack_info(const char *infofile, int force)
{
	struct packed_git *p;
	int stale;
	int i = 0;

	objdir = get_object_directory();
	objdirlen = strlen(objdir);

	prepare_packed_git();
	for (p = packed_git; p; p = p->next) {
		/* we ignore things on alternate path since they are
		 * not available to the pullers in general.
		 */
		if (!p->pack_local)
			continue;
		i++;
	}
	num_pack = i;
	info = xcalloc(num_pack, sizeof(struct pack_info *));
	for (i = 0, p = packed_git; p; p = p->next) {
		if (!p->pack_local)
			continue;
		info[i] = xcalloc(1, sizeof(struct pack_info));
		info[i]->p = p;
		info[i]->old_num = -1;
		i++;
	}

	if (infofile && !force)
		stale = read_pack_info_file(infofile);
	else
		stale = 1;

	for (i = 0; i < num_pack; i++) {
		if (stale) {
			info[i]->old_num = -1;
			info[i]->nr_heads = 0;
		}
	}

	/* renumber them */
	qsort(info, num_pack, sizeof(info[0]), compare_info);
	for (i = 0; i < num_pack; i++)
		info[i]->new_num = i;
}