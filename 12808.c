static int smack_fs_context_dup(struct fs_context *fc,
				struct fs_context *src_fc)
{
	struct smack_mnt_opts *dst, *src = src_fc->security;

	if (!src)
		return 0;

	fc->security = kzalloc(sizeof(struct smack_mnt_opts), GFP_KERNEL);
	if (!fc->security)
		return -ENOMEM;
	dst = fc->security;

	if (src->fsdefault) {
		dst->fsdefault = kstrdup(src->fsdefault, GFP_KERNEL);
		if (!dst->fsdefault)
			return -ENOMEM;
	}
	if (src->fsfloor) {
		dst->fsfloor = kstrdup(src->fsfloor, GFP_KERNEL);
		if (!dst->fsfloor)
			return -ENOMEM;
	}
	if (src->fshat) {
		dst->fshat = kstrdup(src->fshat, GFP_KERNEL);
		if (!dst->fshat)
			return -ENOMEM;
	}
	if (src->fsroot) {
		dst->fsroot = kstrdup(src->fsroot, GFP_KERNEL);
		if (!dst->fsroot)
			return -ENOMEM;
	}
	if (src->fstransmute) {
		dst->fstransmute = kstrdup(src->fstransmute, GFP_KERNEL);
		if (!dst->fstransmute)
			return -ENOMEM;
	}
	return 0;
}