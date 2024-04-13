md_analyze_link_contents(MD_CTX* ctx, const MD_LINE* lines, int n_lines,
                         int mark_beg, int mark_end)
{
    int i;

    md_analyze_marks(ctx, lines, n_lines, mark_beg, mark_end, _T("*_~$@:."));

    for(i = OPENERS_CHAIN_FIRST; i <= OPENERS_CHAIN_LAST; i++) {
        ctx->mark_chains[i].head = -1;
        ctx->mark_chains[i].tail = -1;
    }
}