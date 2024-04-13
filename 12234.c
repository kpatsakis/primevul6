qf_cmdtitle(char_u *cmd)
{
    static char_u qftitle_str[IOSIZE];

    vim_snprintf((char *)qftitle_str, IOSIZE, ":%s", (char *)cmd);
    return qftitle_str;
}