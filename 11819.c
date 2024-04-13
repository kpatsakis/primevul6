chop_out(char *string, unsigned i, unsigned j)
{
    TR(TRACE_DATABASE, ("chop_out %d..%d from %s", i, j, _nc_visbuf(string)));
    while (string[j] != '\0') {
	string[i++] = string[j++];
    }
    string[i] = '\0';
    return i;
}