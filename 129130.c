asmlinkage long compat_sys_fcntl64(unsigned int fd, unsigned int cmd,
		unsigned long arg)
{
	mm_segment_t old_fs;
	struct flock f;
	long ret;

	switch (cmd) {
	case F_GETLK:
	case F_SETLK:
	case F_SETLKW:
		ret = get_compat_flock(&f, compat_ptr(arg));
		if (ret != 0)
			break;
		old_fs = get_fs();
		set_fs(KERNEL_DS);
		ret = sys_fcntl(fd, cmd, (unsigned long)&f);
		set_fs(old_fs);
		if (cmd == F_GETLK && ret == 0) {
			/* GETLK was successfule and we need to return the data...
			 * but it needs to fit in the compat structure.
			 * l_start shouldn't be too big, unless the original
			 * start + end is greater than COMPAT_OFF_T_MAX, in which
			 * case the app was asking for trouble, so we return
			 * -EOVERFLOW in that case.
			 * l_len could be too big, in which case we just truncate it,
			 * and only allow the app to see that part of the conflicting
			 * lock that might make sense to it anyway
			 */

			if (f.l_start > COMPAT_OFF_T_MAX)
				ret = -EOVERFLOW;
			if (f.l_len > COMPAT_OFF_T_MAX)
				f.l_len = COMPAT_OFF_T_MAX;
			if (ret == 0)
				ret = put_compat_flock(&f, compat_ptr(arg));
		}
		break;

	case F_GETLK64:
	case F_SETLK64:
	case F_SETLKW64:
		ret = get_compat_flock64(&f, compat_ptr(arg));
		if (ret != 0)
			break;
		old_fs = get_fs();
		set_fs(KERNEL_DS);
		ret = sys_fcntl(fd, (cmd == F_GETLK64) ? F_GETLK :
				((cmd == F_SETLK64) ? F_SETLK : F_SETLKW),
				(unsigned long)&f);
		set_fs(old_fs);
		if (cmd == F_GETLK64 && ret == 0) {
			/* need to return lock information - see above for commentary */
			if (f.l_start > COMPAT_LOFF_T_MAX)
				ret = -EOVERFLOW;
			if (f.l_len > COMPAT_LOFF_T_MAX)
				f.l_len = COMPAT_LOFF_T_MAX;
			if (ret == 0)
				ret = put_compat_flock64(&f, compat_ptr(arg));
		}
		break;

	default:
		ret = sys_fcntl(fd, cmd, arg);
		break;
	}
	return ret;
}