dwarf_global_die_offset(Dwarf_Global global,
    Dwarf_Off * ret_off, Dwarf_Error * error)
{
    if (global == NULL) {
        _dwarf_error(NULL, error, DW_DLE_GLOBAL_NULL);
        return DW_DLV_ERROR;
    }

    if (global->gl_context == NULL) {
        _dwarf_error(NULL, error, DW_DLE_GLOBAL_CONTEXT_NULL);
        return DW_DLV_ERROR;
    }

    *ret_off = (global->gl_named_die_offset_within_cu +
        global->gl_context->pu_offset_of_cu_header);
    return DW_DLV_OK;
}