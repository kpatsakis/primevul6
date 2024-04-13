int dwarf_return_empty_pubnames(Dwarf_Debug dbg, int flag)
{
    if (dbg == NULL) {
        return DW_DLV_OK;
    }
    if (flag && flag != 1) {
        return DW_DLV_OK;
    }
    dbg->de_return_empty_pubnames = (unsigned char)flag;
    return DW_DLV_OK;
}