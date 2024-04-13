standard_type(const char *name)
{
    int result = -1;
    const struct name_table_entry *np;

    if ((np = _nc_find_entry(name, _nc_get_hash_table(0))) != 0) {
	result = np->nte_type;
    }
    return result;
}