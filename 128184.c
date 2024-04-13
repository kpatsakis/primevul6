do_perror(char *msg)
{
    perror(msg);
    ++emsg_silent;
    emsg(msg);
    --emsg_silent;
}