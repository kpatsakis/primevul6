msg_puts_attr(char *s, int attr)
{
    msg_puts_attr_len(s, -1, attr);
}