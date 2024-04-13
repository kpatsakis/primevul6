md_is_container_mark(MD_CTX* ctx, unsigned indent, OFF beg, OFF* p_end, MD_CONTAINER* p_container)
{
    OFF off = beg;
    OFF max_end;

    if(off >= ctx->size  ||  indent >= ctx->code_indent_offset)
        return FALSE;

    /* Check for block quote mark. */
    if(CH(off) == _T('>')) {
        off++;
        p_container->ch = _T('>');
        p_container->is_loose = FALSE;
        p_container->is_task = FALSE;
        p_container->mark_indent = indent;
        p_container->contents_indent = indent + 1;
        *p_end = off;
        return TRUE;
    }

    /* Check for list item bullet mark. */
    if(ISANYOF(off, _T("-+*"))  &&  (off+1 >= ctx->size || ISBLANK(off+1) || ISNEWLINE(off+1))) {
        p_container->ch = CH(off);
        p_container->is_loose = FALSE;
        p_container->is_task = FALSE;
        p_container->mark_indent = indent;
        p_container->contents_indent = indent + 1;
        *p_end = off+1;
        return TRUE;
    }

    /* Check for ordered list item marks. */
    max_end = off + 9;
    if(max_end > ctx->size)
        max_end = ctx->size;
    p_container->start = 0;
    while(off < max_end  &&  ISDIGIT(off)) {
        p_container->start = p_container->start * 10 + CH(off) - _T('0');
        off++;
    }
    if(off > beg  &&
       (CH(off) == _T('.') || CH(off) == _T(')'))  &&
       (off+1 >= ctx->size || ISBLANK(off+1) || ISNEWLINE(off+1)))
    {
        p_container->ch = CH(off);
        p_container->is_loose = FALSE;
        p_container->is_task = FALSE;
        p_container->mark_indent = indent;
        p_container->contents_indent = indent + off - beg + 1;
        *p_end = off+1;
        return TRUE;
    }

    return FALSE;
}