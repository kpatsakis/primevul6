dwarf_get_globals_header(Dwarf_Global global,
    Dwarf_Off      *pub_section_hdr_offset,
    Dwarf_Unsigned *pub_offset_size,
    Dwarf_Unsigned *pub_cu_length,
    Dwarf_Unsigned *version,
    Dwarf_Off      *info_header_offset,
    Dwarf_Unsigned *info_length,
    Dwarf_Error*   error)
{
    Dwarf_Global_Context con = 0;
    Dwarf_Debug dbg = 0;

    if (global == NULL) {
        _dwarf_error(NULL, error, DW_DLE_GLOBAL_NULL);
        return DW_DLV_ERROR;
    }
    con = global->gl_context;
    if (con == NULL) {
        _dwarf_error(NULL, error, DW_DLE_GLOBAL_CONTEXT_NULL);
        return DW_DLV_ERROR;
    }
    dbg = con->pu_dbg;
    if (!dbg || dbg->de_magic != DBG_IS_VALID) {
        _dwarf_error_string(NULL, error, DW_DLE_DBG_NULL,
            "DW_DLE_DBG_NULL: "
            "calling dwarf_get_globals_header() "
            "either null or it contains"
            "a stale Dwarf_Debug pointer");
        return DW_DLV_ERROR;
    }
    if (pub_section_hdr_offset) {
        *pub_section_hdr_offset = con->pu_pub_offset;
    }
    if (pub_offset_size) {
        *pub_offset_size = con->pu_length_size;
    }
    if (pub_cu_length) {
        *pub_cu_length = con->pu_length;
    }
    if (version) {
        *version = con->pu_version;
    }
    if (info_header_offset) {
        *info_header_offset = con->pu_offset_of_cu_header;
    }
    if (info_length) {
        *info_length = con->pu_info_length;
    }
    return DW_DLV_OK;
}