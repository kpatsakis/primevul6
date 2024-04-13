msg_outtrans_long_attr(char_u *longstr, int attr)
{
    msg_outtrans_long_len_attr(longstr, (int)STRLEN(longstr), attr);
}