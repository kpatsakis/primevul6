md_is_link_reference(MD_CTX* ctx, const MD_LINE* lines, int n_lines,
                     OFF beg, OFF end, MD_LINK_ATTR* attr)
{
    const MD_REF_DEF* def;
    const MD_LINE* beg_line;
    const MD_LINE* end_line;
    CHAR* label;
    SZ label_size;
    int ret;

    MD_ASSERT(CH(beg) == _T('[') || CH(beg) == _T('!'));
    MD_ASSERT(CH(end-1) == _T(']'));

    beg += (CH(beg) == _T('!') ? 2 : 1);
    end--;

    /* Find lines corresponding to the beg and end positions. */
    MD_ASSERT(lines[0].beg <= beg);
    beg_line = lines;
    while(beg >= beg_line->end)
        beg_line++;

    MD_ASSERT(end <= lines[n_lines-1].end);
    end_line = beg_line;
    while(end >= end_line->end)
        end_line++;

    if(beg_line != end_line) {
        MD_CHECK(md_merge_lines_alloc(ctx, beg, end, beg_line,
                 n_lines - (beg_line - lines), _T(' '), &label, &label_size));
    } else {
        label = (CHAR*) STR(beg);
        label_size = end - beg;
    }

    def = md_lookup_ref_def(ctx, label, label_size);
    if(def != NULL) {
        attr->dest_beg = def->dest_beg;
        attr->dest_end = def->dest_end;
        attr->title = def->title;
        attr->title_size = def->title_size;
        attr->title_needs_free = FALSE;
    }

    if(beg_line != end_line)
        free(label);

    ret = (def != NULL);

abort:
    return ret;
}