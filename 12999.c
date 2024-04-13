eat_line(gerb_file_t *fd)
{
    int read;

    do {
	read = gerb_fgetc(fd);
    } while (read != '\n' && read != '\r' && read != EOF);

    /* Restore new line character for processing */
    if (read != EOF)
	gerb_ungetc(fd);
} /* eat_line */