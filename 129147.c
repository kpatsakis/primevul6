pgprot_t vm_get_page_prot(unsigned long vm_flags)
{
	return protection_map[vm_flags &
				(VM_READ|VM_WRITE|VM_EXEC|VM_SHARED)];
}