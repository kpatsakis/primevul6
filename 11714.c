_nc_reserve_pairs(SCREEN *sp, int want)
{
    int have = sp->_pair_alloc;

    if (have == 0)
	have = 1;
    while (have <= want)
	have *= 2;
    if (have > sp->_pair_limit)
	have = sp->_pair_limit;

    if (sp->_color_pairs == 0) {
	sp->_color_pairs = TYPE_CALLOC(colorpair_t, have);
    } else if (have > sp->_pair_alloc) {
#if NCURSES_EXT_COLORS
	colorpair_t *next;

	if ((next = typeCalloc(colorpair_t, have)) == 0)
	    _nc_err_abort(MSG_NO_MEMORY);
	memcpy(next, sp->_color_pairs, (size_t) sp->_pair_alloc * sizeof(*next));
	_nc_copy_pairs(sp, next, sp->_color_pairs, sp->_pair_alloc);
	free(sp->_color_pairs);
	sp->_color_pairs = next;
#else
	TYPE_REALLOC(colorpair_t, have, sp->_color_pairs);
	if (sp->_color_pairs != 0) {
	    memset(sp->_color_pairs + sp->_pair_alloc, 0,
		   sizeof(colorpair_t) * (size_t) (have - sp->_pair_alloc));
	}
#endif
    }
    if (sp->_color_pairs != 0) {
	sp->_pair_alloc = have;
    }
}