long do_arch_prctl_common(struct task_struct *task, int option,
			  unsigned long cpuid_enabled)
{
	switch (option) {
	case ARCH_GET_CPUID:
		return get_cpuid_mode();
	case ARCH_SET_CPUID:
		return set_cpuid_mode(task, cpuid_enabled);
	}

	return -EINVAL;
}