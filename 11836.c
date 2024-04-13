putp(const char *string)
{
    return (tputs(string, 1, _nc_outch));
}