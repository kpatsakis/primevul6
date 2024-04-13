md_is_container_compatible(const MD_CONTAINER* pivot, const MD_CONTAINER* container)
{
    /* Block quote has no "items" like lists. */
    if(container->ch == _T('>'))
        return FALSE;

    if(container->ch != pivot->ch)
        return FALSE;
    if(container->mark_indent > pivot->contents_indent)
        return FALSE;

    return TRUE;
}