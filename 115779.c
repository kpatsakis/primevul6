dwarf_CU_dieoffset_given_die(Dwarf_Die die,
    Dwarf_Off*       return_offset,
    Dwarf_Error*     error)
{
    Dwarf_Off  dieoff = 0;
    Dwarf_CU_Context cucontext = 0;

    CHECK_DIE(die, DW_DLV_ERROR);
    cucontext = die->di_cu_context;
    dieoff =  cucontext->cc_debug_offset;
    /*  The following call cannot fail, so no error check. */
    dwarf_get_cu_die_offset_given_cu_header_offset_b(
        cucontext->cc_dbg, dieoff,
        die->di_is_info, return_offset,error);
    return DW_DLV_OK;
}