dwarf_globname(Dwarf_Global glob,
    char **ret_name,
    Dwarf_Error * error)
{
    if (glob == NULL) {
        _dwarf_error(NULL, error, DW_DLE_GLOBAL_NULL);
        return DW_DLV_ERROR;
    }

    *ret_name = (char *) (glob->gl_name);
    return DW_DLV_OK;
}