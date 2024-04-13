msg_make(char_u *arg)
{
    int	    i;
    static char_u *str = (char_u *)"eeffoc", *rs = (char_u *)"Plon#dqg#vxjduB";

    arg = skipwhite(arg);
    for (i = 5; *arg && i >= 0; --i)
	if (*arg++ != str[i])
	    break;
    if (i < 0)
    {
	msg_putchar('\n');
	for (i = 0; rs[i]; ++i)
	    msg_putchar(rs[i] - 3);
    }
}