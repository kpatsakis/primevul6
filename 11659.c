lookup_user_capability(const char *name)
{
    struct user_table_entry const *result = 0;
    if (*name != 'k') {
	result = _nc_find_user_entry(name);
    }
    return result;
}