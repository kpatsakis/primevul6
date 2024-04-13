static int dump_seek(struct file *file, loff_t off)
{
	if (file->f_op->llseek) {
		if (file->f_op->llseek(file, off, SEEK_SET) != off)
			return 0;
	} else {
		file->f_pos = off;
	}
	return 1;
}