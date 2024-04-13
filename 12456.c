static inline unsigned FNAME(gpte_access)(u64 gpte)
{
	unsigned access;
#if PTTYPE == PTTYPE_EPT
	access = ((gpte & VMX_EPT_WRITABLE_MASK) ? ACC_WRITE_MASK : 0) |
		((gpte & VMX_EPT_EXECUTABLE_MASK) ? ACC_EXEC_MASK : 0) |
		((gpte & VMX_EPT_READABLE_MASK) ? ACC_USER_MASK : 0);
#else
	BUILD_BUG_ON(ACC_EXEC_MASK != PT_PRESENT_MASK);
	BUILD_BUG_ON(ACC_EXEC_MASK != 1);
	access = gpte & (PT_WRITABLE_MASK | PT_USER_MASK | PT_PRESENT_MASK);
	/* Combine NX with P (which is set here) to get ACC_EXEC_MASK.  */
	access ^= (gpte >> PT64_NX_SHIFT);
#endif

	return access;
}