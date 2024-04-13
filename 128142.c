msg_outtrans_attr(char_u *str, int attr)
{
    return msg_outtrans_len_attr(str, (int)STRLEN(str), attr);
}