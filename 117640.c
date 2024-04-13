md_link_label_hash(const CHAR* label, SZ size)
{
    unsigned hash = MD_FNV1A_BASE;
    OFF off;
    unsigned codepoint;
    int is_whitespace = FALSE;

    off = md_skip_unicode_whitespace(label, 0, size);
    while(off < size) {
        SZ char_size;

        codepoint = md_decode_unicode(label, off, size, &char_size);
        is_whitespace = ISUNICODEWHITESPACE_(codepoint) || ISNEWLINE_(label[off]);

        if(is_whitespace) {
            codepoint = ' ';
            hash = md_fnv1a(hash, &codepoint, sizeof(unsigned));
            off = md_skip_unicode_whitespace(label, off, size);
        } else {
            MD_UNICODE_FOLD_INFO fold_info;

            md_get_unicode_fold_info(codepoint, &fold_info);
            hash = md_fnv1a(hash, fold_info.codepoints, fold_info.n_codepoints * sizeof(unsigned));
            off += char_size;
        }
    }

    return hash;
}