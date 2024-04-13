save_char(char *s, int c)
{
    static char temp[2];
    temp[0] = (char) c;
    return save_string(s, temp);
}