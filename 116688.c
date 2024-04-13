static int update_info_packs(int force)
{
	char infofile[PATH_MAX];
	char name[PATH_MAX];
	int namelen;
	FILE *fp;

	namelen = sprintf(infofile, "%s/info/packs", get_object_directory());
	strcpy(name, infofile);
	strcpy(name + namelen, "+");

	init_pack_info(infofile, force);

	safe_create_leading_directories(name);
	fp = fopen(name, "w");
	if (!fp)
		return error("cannot open %s", name);
	write_pack_info_file(fp);
	fclose(fp);
	adjust_shared_perm(name);
	rename(name, infofile);
	return 0;
}