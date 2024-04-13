static int put_compat_statfs64(struct compat_statfs64 __user *ubuf, struct kstatfs *kbuf)
{
	if (sizeof ubuf->f_blocks == 4) {
		if ((kbuf->f_blocks | kbuf->f_bfree | kbuf->f_bavail) &
		    0xffffffff00000000ULL)
			return -EOVERFLOW;
		/* f_files and f_ffree may be -1; it's okay
		 * to stuff that into 32 bits */
		if (kbuf->f_files != 0xffffffffffffffffULL
		 && (kbuf->f_files & 0xffffffff00000000ULL))
			return -EOVERFLOW;
		if (kbuf->f_ffree != 0xffffffffffffffffULL
		 && (kbuf->f_ffree & 0xffffffff00000000ULL))
			return -EOVERFLOW;
	}
	if (!access_ok(VERIFY_WRITE, ubuf, sizeof(*ubuf)) ||
	    __put_user(kbuf->f_type, &ubuf->f_type) ||
	    __put_user(kbuf->f_bsize, &ubuf->f_bsize) ||
	    __put_user(kbuf->f_blocks, &ubuf->f_blocks) ||
	    __put_user(kbuf->f_bfree, &ubuf->f_bfree) ||
	    __put_user(kbuf->f_bavail, &ubuf->f_bavail) ||
	    __put_user(kbuf->f_files, &ubuf->f_files) ||
	    __put_user(kbuf->f_ffree, &ubuf->f_ffree) ||
	    __put_user(kbuf->f_namelen, &ubuf->f_namelen) ||
	    __put_user(kbuf->f_fsid.val[0], &ubuf->f_fsid.val[0]) ||
	    __put_user(kbuf->f_fsid.val[1], &ubuf->f_fsid.val[1]) ||
	    __put_user(kbuf->f_frsize, &ubuf->f_frsize))
		return -EFAULT;
	return 0;
}