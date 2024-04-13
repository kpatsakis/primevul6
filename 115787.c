dwarf_get_globals(Dwarf_Debug dbg,
    Dwarf_Global ** globals,
    Dwarf_Signed * return_count, Dwarf_Error * error)
{
    int res = 0;

    if (!dbg || dbg->de_magic != DBG_IS_VALID) {
        _dwarf_error_string(NULL, error, DW_DLE_DBG_NULL,
            "DW_DLE_DBG_NULL: "
            "calling dwarf_get_globals "
            "Dwarf_Debug either null or it is"
            "a stale Dwarf_Debug pointer");
        return DW_DLV_ERROR;
    }
    res = _dwarf_load_section(dbg, &dbg->de_debug_pubnames,error);
    if (res != DW_DLV_OK) {
        return res;
    }
    if (!dbg->de_debug_pubnames.dss_size) {
        return DW_DLV_NO_ENTRY;
    }

    res = _dwarf_internal_get_pubnames_like_data(dbg,
        ".debug_pubnames",
        dbg->de_debug_pubnames.dss_data,
        dbg->de_debug_pubnames.dss_size,
        globals,
        return_count,
        error,
        DW_DLA_GLOBAL_CONTEXT,
        DW_DLA_GLOBAL,
        DW_DLE_PUBNAMES_LENGTH_BAD,
        DW_DLE_PUBNAMES_VERSION_ERROR);
    return res;

}