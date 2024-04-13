cat_file(char *file)
{
    FILE *fp;
    size_t nr;
    char buf[BUFSIZ];
    bool sent = FALSE;

    if (file != 0) {
	if ((fp = fopen(file, "r")) == 0)
	    failed(file);

	while ((nr = fread(buf, sizeof(char), sizeof(buf), fp)) != 0) {
	    if (fwrite(buf, sizeof(char), nr, my_file) != nr) {
		failed(file);
	    }
	    sent = TRUE;
	}
	fclose(fp);
    }
    return sent;
}