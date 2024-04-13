md_line_indentation(MD_CTX* ctx, unsigned total_indent, OFF beg, OFF* p_end)
{
    OFF off = beg;
    unsigned indent = total_indent;

    while(off < ctx->size  &&  ISBLANK(off)) {
        if(CH(off) == _T('\t'))
            indent = (indent + 4) & ~3;
        else
            indent++;
        off++;
    }

    *p_end = off;
    return indent - total_indent;
}