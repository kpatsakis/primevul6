msg_outnum(long n)
{
    char	buf[20];

    sprintf(buf, "%ld", n);
    msg_puts(buf);
}