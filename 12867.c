static int selinux_kernel_load_data(enum kernel_load_data_id id, bool contents)
{
	int rc = 0;

	switch (id) {
	case LOADING_MODULE:
		rc = selinux_kernel_module_from_file(NULL);
		break;
	default:
		break;
	}

	return rc;
}