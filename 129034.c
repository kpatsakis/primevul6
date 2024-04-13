static void flush_old_files(struct files_struct * files)
{
	long j = -1;
	struct fdtable *fdt;

	spin_lock(&files->file_lock);
	for (;;) {
		unsigned long set, i;

		j++;
		i = j * __NFDBITS;
		fdt = files_fdtable(files);
		if (i >= fdt->max_fds)
			break;
		set = fdt->close_on_exec->fds_bits[j];
		if (!set)
			continue;
		fdt->close_on_exec->fds_bits[j] = 0;
		spin_unlock(&files->file_lock);
		for ( ; set ; i++,set >>= 1) {
			if (set & 1) {
				sys_close(i);
			}
		}
		spin_lock(&files->file_lock);

	}
	spin_unlock(&files->file_lock);
}