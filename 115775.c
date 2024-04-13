pubnames_error_length(Dwarf_Debug dbg,
    Dwarf_Error *error,
    Dwarf_Unsigned spaceneeded,
    const char *secname,
    const char *specificloc)
{
    dwarfstring m;

    dwarfstring_constructor(&m);
    dwarfstring_append(&m,"DW_DLE_PUBNAMES_LENGTH_BAD: "
        " In section ");
    dwarfstring_append(&m,(char *)secname);
    dwarfstring_append_printf_u(&m,
        " %u bytes of space needed "
        "but the section is out of space ",
        spaceneeded);
    dwarfstring_append(&m, "reading ");
    dwarfstring_append(&m, (char *)specificloc);
    dwarfstring_append(&m, ".");
    _dwarf_error_string(dbg,error,DW_DLE_PUBNAMES_LENGTH_BAD,
        dwarfstring_string(&m));
    dwarfstring_destructor(&m);
}