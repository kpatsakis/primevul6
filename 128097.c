verb_msg(char *s)
{
    int		n;

    verbose_enter();
    n = msg_attr_keep(s, 0, FALSE);
    verbose_leave();

    return n;
}