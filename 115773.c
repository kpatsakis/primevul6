build_off_end_msg(Dwarf_Unsigned offval,
    Dwarf_Unsigned withincr,
    Dwarf_Unsigned secsize,
    dwarfstring *m)
{
    const char *msg = "past";
    if (offval < secsize){
        msg = "too near";
    }
    dwarfstring_append_printf_u(m,"DW_DLE_OFFSET_BAD: "
        "The CU header offset of %u in a pubnames-like entry ",
        withincr);
    dwarfstring_append_printf_s(m,
        "would put us %s the end of .debug_info. "
        "No room for a DIE there... "
        "Corrupt Dwarf.",(char *)msg);
    return;
}