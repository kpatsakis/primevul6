md_skip_unicode_whitespace(const CHAR* label, OFF off, SZ size)
{
    SZ char_size;
    unsigned codepoint;

    while(off < size) {
        codepoint = md_decode_unicode(label, off, size, &char_size);
        if(!ISUNICODEWHITESPACE_(codepoint)  &&  !ISNEWLINE_(label[off]))
            break;
        off += char_size;
    }

    return off;
}