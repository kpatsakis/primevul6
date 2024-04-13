dwarf_globals_dealloc(Dwarf_Debug dbg, Dwarf_Global * dwgl,
    Dwarf_Signed count)
{
    _dwarf_internal_globals_dealloc(dbg, dwgl, count);
    return;
}