exit_code(int token, int value)
{
    int result = 99;

    switch (token) {
    case BOOLEAN:
	result = !value;	/* TRUE=0, FALSE=1 */
	break;
    case NUMBER:
	result = 0;		/* always zero */
	break;
    case STRING:
	result = value;		/* 0=normal, 1=missing */
	break;
    }
    return result;
}