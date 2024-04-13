vm_unmapped_area(struct vm_unmapped_area_info *info)
{
	if (info->flags & VM_UNMAPPED_AREA_TOPDOWN)
		return unmapped_area_topdown(info);
	else
		return unmapped_area(info);
}