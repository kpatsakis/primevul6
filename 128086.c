static int acp_poweron(struct generic_pm_domain *genpd)
{
	struct acp_pm_domain *apd;
	struct amdgpu_device *adev;

	apd = container_of(genpd, struct acp_pm_domain, gpd);
	if (apd != NULL) {
		adev = apd->adev;
	/* call smu to UNGATE ACP block
	 * smu will
	 * 1. exit ulv
	 * 2. turn on acp clock
	 * 3. power on acp tiles
	 */
		if (adev->powerplay.pp_funcs->set_powergating_by_smu)
			amdgpu_dpm_set_powergating_by_smu(adev, AMD_IP_BLOCK_TYPE_ACP, false);
	}
	return 0;
}