static void *do_smb_super_data_conv(void *raw_data)
{
	struct smb_mount_data *s = raw_data;
	struct compat_smb_mount_data *c_s = raw_data;

	if (c_s->version != SMB_MOUNT_OLDVERSION)
		goto out;
	s->dir_mode = c_s->dir_mode;
	s->file_mode = c_s->file_mode;
	s->gid = c_s->gid;
	s->uid = c_s->uid;
	s->mounted_uid = c_s->mounted_uid;
 out:
	return raw_data;
}