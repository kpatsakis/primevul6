static inline void htx_reset(struct htx *htx)
{
	htx->tail = htx->head  = htx->first = -1;
	htx->data = 0;
	htx->tail_addr = htx->head_addr = htx->end_addr = 0;
	htx->extra = 0;
	htx->flags = HTX_FL_NONE;
}