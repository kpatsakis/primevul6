static int dump_seek(struct file *file, loff_t off)
{
	if (file->f_op->llseek && file->f_op->llseek != no_llseek) {
		if (file->f_op->llseek(file, off, SEEK_CUR) < 0)
			return 0;
	} else {
		char *buf = (char *)get_zeroed_page(GFP_KERNEL);
		if (!buf)
			return 0;
		while (off > 0) {
			unsigned long n = off;
			if (n > PAGE_SIZE)
				n = PAGE_SIZE;
			if (!dump_write(file, buf, n))
				return 0;
			off -= n;
		}
		free_page((unsigned long)buf);
	}
	return 1;
}