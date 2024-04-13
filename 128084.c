static int acp_poweroff(struct generic_pm_domain *genpd)
{
	struct acp_pm_domain *apd;
	struct amdgpu_device *adev;

	apd = container_of(genpd, struct acp_pm_domain, gpd);
	if (apd != NULL) {
		adev = apd->adev;
	/* call smu to POWER GATE ACP block
	 * smu will
	 * 1. turn off the acp clock
	 * 2. power off the acp tiles
	 * 3. check and enter ulv state
	 */
		if (adev->powerplay.pp_funcs &&
			adev->powerplay.pp_funcs->set_powergating_by_smu)
			amdgpu_dpm_set_powergating_by_smu(adev, AMD_IP_BLOCK_TYPE_ACP, true);
	}
	return 0;
}