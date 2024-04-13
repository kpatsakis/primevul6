md_link_label_cmp(const CHAR* a_label, SZ a_size, const CHAR* b_label, SZ b_size)
{
    OFF a_off;
    OFF b_off;
    MD_UNICODE_FOLD_INFO a_fi = { { 0 }, 0 };
    MD_UNICODE_FOLD_INFO b_fi = { { 0 }, 0 };
    OFF a_fi_off = 0;
    OFF b_fi_off = 0;
    int cmp;

    a_off = md_skip_unicode_whitespace(a_label, 0, a_size);
    b_off = md_skip_unicode_whitespace(b_label, 0, b_size);
    while(a_off < a_size || a_fi_off < a_fi.n_codepoints ||
          b_off < b_size || b_fi_off < b_fi.n_codepoints)
    {
        /* If needed, load fold info for next char. */
        if(a_fi_off >= a_fi.n_codepoints) {
            a_fi_off = 0;
            a_off = md_link_label_cmp_load_fold_info(a_label, a_off, a_size, &a_fi);
        }
        if(b_fi_off >= b_fi.n_codepoints) {
            b_fi_off = 0;
            b_off = md_link_label_cmp_load_fold_info(b_label, b_off, b_size, &b_fi);
        }

        cmp = b_fi.codepoints[b_fi_off] - a_fi.codepoints[a_fi_off];
        if(cmp != 0)
            return cmp;

        a_fi_off++;
        b_fi_off++;
    }

    return 0;
}