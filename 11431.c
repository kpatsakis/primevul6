prof_report(char *name)
{
    FILE *fd = fopen("suggestprof", "a");

    profile_end(&total);
    fprintf(fd, "-----------------------\n");
    fprintf(fd, "%s: %s\n", name, profile_msg(&total));
    for (int i = 0; i <= STATE_FINAL; ++i)
	fprintf(fd, "%d: %s (%ld)\n", i, profile_msg(&times[i]), counts[i]);
    fclose(fd);
}