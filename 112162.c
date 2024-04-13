CreateOptsFile(int argc, char *argv[], char *fullprogname)
{
	FILE	   *fp;
	int			i;

#define OPTS_FILE	"postmaster.opts"

	if ((fp = fopen(OPTS_FILE, "w")) == NULL)
	{
		ereport(LOG,
				(errcode_for_file_access(),
				 errmsg("could not create file \"%s\": %m", OPTS_FILE)));
		return false;
	}

	fprintf(fp, "%s", fullprogname);
	for (i = 1; i < argc; i++)
		fprintf(fp, " \"%s\"", argv[i]);
	fputs("\n", fp);

	if (fclose(fp))
	{
		ereport(LOG,
				(errcode_for_file_access(),
				 errmsg("could not write file \"%s\": %m", OPTS_FILE)));
		return false;
	}

	return true;
}