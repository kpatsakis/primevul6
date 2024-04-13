static int strlenext(char *str)
{
	int i=0;
	while (*str)
	{
		i++;
		if (*str=='\'') i+=BSC;
			str++;	
	}
	return i;
}