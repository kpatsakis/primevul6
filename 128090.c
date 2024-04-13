static int acp_sw_fini(void *handle)
{
	struct amdgpu_device *adev = (struct amdgpu_device *)handle;

	if (adev->acp.cgs_device)
		amdgpu_cgs_destroy_device(adev->acp.cgs_device);

	return 0;
}