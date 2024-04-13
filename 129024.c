static inline int open_arg(int flags, int mask)
{
	int n = ACC_MODE(flags);
	if (flags & (O_TRUNC | O_CREAT))
		n |= AUDIT_PERM_WRITE;
	return n & mask;
}