ia32_setup_arg_pages (struct linux_binprm *bprm, int executable_stack)
{
	int ret;

	ret = setup_arg_pages(bprm, IA32_STACK_TOP, executable_stack);
	if (!ret) {
		/*
		 * Can't do it in ia64_elf32_init(). Needs to be done before
		 * calls to elf32_map()
		 */
		current->thread.ppl = ia32_init_pp_list();
	}

	return ret;
}