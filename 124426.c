i64 get_ram(rzip_control *control)
{
	i64 ramsize;
	FILE *meminfo;
	char aux[256];

	ramsize = (i64)sysconf(_SC_PHYS_PAGES) * PAGE_SIZE;
	if (ramsize > 0)
		return ramsize;

	/* Workaround for uclibc which doesn't properly support sysconf */
	if(!(meminfo = fopen("/proc/meminfo", "r")))
		fatal_return(("fopen\n"), -1);

	while(!feof(meminfo) && !fscanf(meminfo, "MemTotal: %"PRId64" kB", &ramsize)) {
		if (unlikely(fgets(aux, sizeof(aux), meminfo) == NULL)) {
			fclose(meminfo);
			fatal_return(("Failed to fgets in get_ram\n"), -1);
		}
	}
	if (fclose(meminfo) == -1)
		fatal_return(("fclose"), -1);
	ramsize *= 1000;

	return ramsize;
}