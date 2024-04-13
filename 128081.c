static int acp_sw_init(void *handle)
{
	struct amdgpu_device *adev = (struct amdgpu_device *)handle;

	adev->acp.parent = adev->dev;

	adev->acp.cgs_device =
		amdgpu_cgs_create_device(adev);
	if (!adev->acp.cgs_device)
		return -EINVAL;

	return 0;
}