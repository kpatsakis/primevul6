_dwarf_fix_up_offset_irix(Dwarf_Debug dbg,
    Dwarf_Unsigned * varp, char *caller_site_name)
{

    Dwarf_Unsigned var = *varp;

#define UPPER33 0xffffffff80000000LL
#define LOWER32         0xffffffffLL
    /*  Restrict the hack to the known case. Upper 32 bits erroneously
        sign extended from lower 32 upper bit. */
    if ((var & UPPER33) == UPPER33) {
        var &= LOWER32;
        /* Apply the fix. Dreadful hack. */
        *varp = var;
    }
#undef UPPER33
#undef LOWER32
    return;
}