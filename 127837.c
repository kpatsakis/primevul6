decode(unsigned char **cpp)
{
	register int x;

	x = *(*cpp)++;
	if(x == 0){
		return pull16(cpp) & 0xffff;	/* pull16 returns -1 on error */
	} else {
		return x & 0xff;		/* -1 if PULLCHAR returned error */
	}
}