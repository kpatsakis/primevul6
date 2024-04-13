static int selinux_kernel_read_file(struct file *file,
				    enum kernel_read_file_id id,
				    bool contents)
{
	int rc = 0;

	switch (id) {
	case READING_MODULE:
		rc = selinux_kernel_module_from_file(contents ? file : NULL);
		break;
	default:
		break;
	}

	return rc;
}