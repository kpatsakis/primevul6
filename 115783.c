_dwarf_make_global_add_to_chain(Dwarf_Debug dbg,
    Dwarf_Unsigned       global_DLA_code,
    Dwarf_Global_Context pubnames_context,
    Dwarf_Off            die_offset_in_cu,
    unsigned char   *    glname,
    Dwarf_Unsigned      *global_count,
    Dwarf_Bool          *pubnames_context_on_list,
    Dwarf_Chain         **plast_chain,
    Dwarf_Error         *error)
{
    Dwarf_Chain  curr_chain = 0;
    Dwarf_Global global = 0;

    global = (Dwarf_Global)
        _dwarf_get_alloc(dbg, global_DLA_code, 1);
    if (!global) {
        _dwarf_error(dbg, error, DW_DLE_ALLOC_FAIL);
        return DW_DLV_ERROR;
    }
    (*global_count)++;
    /*  Recording the same context in another Dwarf_Global */
    global->gl_context = pubnames_context;
    global->gl_alloc_type = global_DLA_code;
    global->gl_named_die_offset_within_cu = die_offset_in_cu;
    global->gl_name = glname;
    /* Finish off current entry chain */
    curr_chain =
        (Dwarf_Chain) _dwarf_get_alloc(dbg, DW_DLA_CHAIN, 1);
    if (!curr_chain) {
        dwarf_dealloc(dbg,global,global_DLA_code);
        _dwarf_error(dbg, error, DW_DLE_ALLOC_FAIL);
        return DW_DLV_ERROR;
    }
    /* Put current global on singly_linked list. */
    curr_chain->ch_item = (Dwarf_Global) global;
    curr_chain->ch_itemtype = global_DLA_code;
    (**plast_chain) = curr_chain;
    *plast_chain = &(curr_chain->ch_next);
    *pubnames_context_on_list = TRUE;
    return DW_DLV_OK;
}