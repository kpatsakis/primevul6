dwarf_get_cu_die_offset_given_cu_header_offset_b(Dwarf_Debug dbg,
    Dwarf_Off in_cu_header_offset,
    Dwarf_Bool is_info,
    Dwarf_Off * out_cu_die_offset,
    Dwarf_Error * error)
{
    Dwarf_Off headerlen = 0;
    int cres = 0;

    if (!dbg || dbg->de_magic != DBG_IS_VALID) {
        _dwarf_error_string(NULL, error, DW_DLE_DBG_NULL,
            "DW_DLE_DBG_NULL: "
            "calling dwarf_get_cu_die_offset_given"
            "cu_header_offset_b Dwarf_Debug is"
            "either null or it is"
            "a stale Dwarf_Debug pointer");
        return DW_DLV_ERROR;
    }
    cres = _dwarf_length_of_cu_header(dbg,
        in_cu_header_offset,is_info, &headerlen,error);
    if (cres != DW_DLV_OK) {
        return cres;
    }
    *out_cu_die_offset = in_cu_header_offset + headerlen;
    return DW_DLV_OK;
}