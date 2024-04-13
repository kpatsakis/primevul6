static inline void compat_nfs_string(struct nfs_string *dst,
				     struct compat_nfs_string *src)
{
	dst->data = compat_ptr(src->data);
	dst->len = src->len;
}