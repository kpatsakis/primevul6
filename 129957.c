mono_class_free_ref_info (MonoClass *klass)
{
	if (klass->ref_info_handle) {
		mono_gchandle_free (klass->ref_info_handle);
		klass->ref_info_handle = 0;
	}
}