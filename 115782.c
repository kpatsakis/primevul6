dwarf_global_cu_offset(Dwarf_Global global,
    Dwarf_Off * cu_header_offset,
    Dwarf_Error * error)
{
    Dwarf_Global_Context con = 0;

    if (global == NULL) {
        _dwarf_error(NULL, error, DW_DLE_GLOBAL_NULL);
        return DW_DLV_ERROR;
    }
    con = global->gl_context;
    if (con == NULL) {
        _dwarf_error(NULL, error, DW_DLE_GLOBAL_CONTEXT_NULL);
        return DW_DLV_ERROR;
    }
    *cu_header_offset = con->pu_offset_of_cu_header;
    return DW_DLV_OK;
}