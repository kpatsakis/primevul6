free_namelist(char **src)
{
    if (src != 0) {
	int n;
	for (n = 0; src[n] != 0; ++n)
	    free(src[n]);
	free(src);
    }
}