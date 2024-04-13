_dwarf_internal_globals_dealloc(Dwarf_Debug dbg,
    Dwarf_Global * dwgl,
    Dwarf_Signed count)
{
    Dwarf_Signed i = 0;
    struct Dwarf_Global_Context_s *glcp = 0;
    struct Dwarf_Global_Context_s *lastglcp = 0;

    if (!dwgl) {
        return;
    }
    for (i = 0; i < count; i++) {
        Dwarf_Global dgd = dwgl[i];

        if (!dgd) {
            continue;
        }
        /*  Avoids duplicate frees of repeated
            use of contexts (while assuming that
            all uses of a particular gl_context
            will appear next to each other. */
        glcp = dgd->gl_context;
        if (glcp && lastglcp != glcp) {
            lastglcp = glcp;
            dwarf_dealloc(dbg, glcp, glcp->pu_alloc_type);
        }
        dwarf_dealloc(dbg, dgd, dgd->gl_alloc_type);
    }
    dwarf_dealloc(dbg, dwgl, DW_DLA_LIST);
    return;
}