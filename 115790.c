dealloc_globals_chain(Dwarf_Debug dbg,
    Dwarf_Chain head_chain)
{
    Dwarf_Chain curr_chain = 0;
    int chaintype = DW_DLA_CHAIN;
    Dwarf_Global_Context lastcontext = 0;
    Dwarf_Global_Context curcontext = 0;

    curr_chain = head_chain;
    for (; curr_chain; ) {
        Dwarf_Global item = 0;
        int itemtype = 0;
        Dwarf_Chain prev = 0;

        item = (Dwarf_Global)curr_chain->ch_item;
        itemtype = curr_chain->ch_itemtype;
        curcontext = item->gl_context;
        if (curcontext && curcontext != lastcontext) {
            /* First time we see a context, dealloc it. */
            lastcontext = curcontext;
            dwarf_dealloc(dbg,curcontext,curcontext->pu_alloc_type);
        }
        prev = curr_chain;
        dwarf_dealloc(dbg, item,itemtype);
        prev->ch_item = 0;
        curr_chain = curr_chain->ch_next;
        dwarf_dealloc(dbg, prev, chaintype);
    }
}