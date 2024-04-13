static int put_compat_flock(struct flock *kfl, struct compat_flock __user *ufl)
{
	if (!access_ok(VERIFY_WRITE, ufl, sizeof(*ufl)) ||
	    __put_user(kfl->l_type, &ufl->l_type) ||
	    __put_user(kfl->l_whence, &ufl->l_whence) ||
	    __put_user(kfl->l_start, &ufl->l_start) ||
	    __put_user(kfl->l_len, &ufl->l_len) ||
	    __put_user(kfl->l_pid, &ufl->l_pid))
		return -EFAULT;
	return 0;
}