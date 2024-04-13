md_analyze_table_cell_boundary(MD_CTX* ctx, int mark_index)
{
    MD_MARK* mark = &ctx->marks[mark_index];
    mark->flags |= MD_MARK_RESOLVED;

    md_mark_chain_append(ctx, &TABLECELLBOUNDARIES, mark_index);
    ctx->n_table_cell_boundaries++;
}