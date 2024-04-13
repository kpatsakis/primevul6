md_link_label_cmp_load_fold_info(const CHAR* label, OFF off, SZ size,
                                 MD_UNICODE_FOLD_INFO* fold_info)
{
    unsigned codepoint;
    SZ char_size;

    if(off >= size) {
        /* Treat end of a link label as a whitespace. */
        goto whitespace;
    }

    codepoint = md_decode_unicode(label, off, size, &char_size);
    off += char_size;
    if(ISUNICODEWHITESPACE_(codepoint)) {
        /* Treat all whitespace as equivalent */
        goto whitespace;
    }

    /* Get real folding info. */
    md_get_unicode_fold_info(codepoint, fold_info);
    return off;

whitespace:
    fold_info->codepoints[0] = _T(' ');
    fold_info->n_codepoints = 1;
    return md_skip_unicode_whitespace(label, off, size);
}