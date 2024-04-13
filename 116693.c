static struct pack_info *find_pack_by_name(const char *name)
{
	int i;
	for (i = 0; i < num_pack; i++) {
		struct packed_git *p = info[i]->p;
		/* skip "/pack/" after ".git/objects" */
		if (!strcmp(p->pack_name + objdirlen + 6, name))
			return info[i];
	}
	return NULL;
}