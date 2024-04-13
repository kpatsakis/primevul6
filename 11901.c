test_tparm(const char *name, int *number)
{
    char *format = tigetstr(name);
    if ((format = validate(name)) != 0) {
	char *result = tparm(format,
			     number[0],
			     number[1],
			     number[2],
			     number[3],
			     number[4],
			     number[5],
			     number[6],
			     number[7],
			     number[8]);
	if (v_opt > 1)
	    printf(".. %2d = %2d %2d %2d %2d %2d %2d %2d %2d %2d %s\n",
		   result != 0 ? (int) strlen(result) : -1,
		   number[0],
		   number[1],
		   number[2],
		   number[3],
		   number[4],
		   number[5],
		   number[6],
		   number[7],
		   number[8],
		   name);
    }
}