static inline u32 file_to_av(struct file *file)
{
	u32 av = 0;

	if (file->f_mode & FMODE_READ)
		av |= FILE__READ;
	if (file->f_mode & FMODE_WRITE) {
		if (file->f_flags & O_APPEND)
			av |= FILE__APPEND;
		else
			av |= FILE__WRITE;
	}
	if (!av) {
		/*
		 * Special file opened with flags 3 for ioctl-only use.
		 */
		av = FILE__IOCTL;
	}

	return av;
}