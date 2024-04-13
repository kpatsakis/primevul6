freeArgv(char *blob, char **argv)
{
    if (blob) {
	free(blob);
	if (argv) {
	    int n;
	    for (n = 0; argv[n]; ++n)
		free(argv[n]);
	    free(argv);
	}
    }
}