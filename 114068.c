static inline int htx_expect_more(const struct htx *htx)
{
	return !(htx->flags & HTX_FL_EOM);
}