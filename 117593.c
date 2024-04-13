md_is_link_reference_definition(MD_CTX* ctx, const MD_LINE* lines, int n_lines)
{
    OFF label_contents_beg;
    OFF label_contents_end;
    int label_contents_line_index = -1;
    int label_is_multiline = FALSE;
    OFF dest_contents_beg;
    OFF dest_contents_end;
    OFF title_contents_beg;
    OFF title_contents_end;
    int title_contents_line_index;
    int title_is_multiline = FALSE;
    OFF off;
    int line_index = 0;
    int tmp_line_index;
    MD_REF_DEF* def = NULL;
    int ret = 0;

    /* Link label. */
    if(!md_is_link_label(ctx, lines, n_lines, lines[0].beg,
                &off, &label_contents_line_index, &line_index,
                &label_contents_beg, &label_contents_end))
        return FALSE;
    label_is_multiline = (label_contents_line_index != line_index);

    /* Colon. */
    if(off >= lines[line_index].end  ||  CH(off) != _T(':'))
        return FALSE;
    off++;

    /* Optional white space with up to one line break. */
    while(off < lines[line_index].end  &&  ISWHITESPACE(off))
        off++;
    if(off >= lines[line_index].end) {
        line_index++;
        if(line_index >= n_lines)
            return FALSE;
        off = lines[line_index].beg;
    }

    /* Link destination. */
    if(!md_is_link_destination(ctx, off, lines[line_index].end,
                &off, &dest_contents_beg, &dest_contents_end))
        return FALSE;

    /* (Optional) title. Note we interpret it as an title only if nothing
     * more follows on its last line. */
    if(md_is_link_title(ctx, lines + line_index, n_lines - line_index, off,
                &off, &title_contents_line_index, &tmp_line_index,
                &title_contents_beg, &title_contents_end)
        &&  off >= lines[line_index + tmp_line_index].end)
    {
        title_is_multiline = (tmp_line_index != title_contents_line_index);
        title_contents_line_index += line_index;
        line_index += tmp_line_index;
    } else {
        /* Not a title. */
        title_is_multiline = FALSE;
        title_contents_beg = off;
        title_contents_end = off;
        title_contents_line_index = 0;
    }

    /* Nothing more can follow on the last line. */
    if(off < lines[line_index].end)
        return FALSE;

    /* So, it _is_ a reference definition. Remember it. */
    if(ctx->n_ref_defs >= ctx->alloc_ref_defs) {
        MD_REF_DEF* new_defs;

        ctx->alloc_ref_defs = (ctx->alloc_ref_defs > 0
                ? ctx->alloc_ref_defs + ctx->alloc_ref_defs / 2
                : 16);
        new_defs = (MD_REF_DEF*) realloc(ctx->ref_defs, ctx->alloc_ref_defs * sizeof(MD_REF_DEF));
        if(new_defs == NULL) {
            MD_LOG("realloc() failed.");
            goto abort;
        }

        ctx->ref_defs = new_defs;
    }
    def = &ctx->ref_defs[ctx->n_ref_defs];
    memset(def, 0, sizeof(MD_REF_DEF));

    if(label_is_multiline) {
        MD_CHECK(md_merge_lines_alloc(ctx, label_contents_beg, label_contents_end,
                    lines + label_contents_line_index, n_lines - label_contents_line_index,
                    _T(' '), &def->label, &def->label_size));
        def->label_needs_free = TRUE;
    } else {
        def->label = (CHAR*) STR(label_contents_beg);
        def->label_size = label_contents_end - label_contents_beg;
    }

    if(title_is_multiline) {
        MD_CHECK(md_merge_lines_alloc(ctx, title_contents_beg, title_contents_end,
                    lines + title_contents_line_index, n_lines - title_contents_line_index,
                    _T('\n'), &def->title, &def->title_size));
        def->title_needs_free = TRUE;
    } else {
        def->title = (CHAR*) STR(title_contents_beg);
        def->title_size = title_contents_end - title_contents_beg;
    }

    def->dest_beg = dest_contents_beg;
    def->dest_end = dest_contents_end;

    /* Success. */
    ctx->n_ref_defs++;
    return line_index + 1;

abort:
    /* Failure. */
    if(def != NULL  &&  def->label_needs_free)
        free(def->label);
    if(def != NULL  &&  def->title_needs_free)
        free(def->title);
    return ret;
}