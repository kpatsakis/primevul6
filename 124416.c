static bool read_tmpinmagic(rzip_control *control)
{
	char magic[MAGIC_LEN];
	int i, tmpchar;

	memset(magic, 0, sizeof(magic));
	for (i = 0; i < 24; i++) {
		tmpchar = getchar();
		if (unlikely(tmpchar == EOF))
			failure_return(("Reached end of file on STDIN prematurely on v05 magic read\n"), false);
		magic[i] = (char)tmpchar;
	}
	return get_magic(control, magic);
}