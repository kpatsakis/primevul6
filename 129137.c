elf32_set_personality (void)
{
	set_personality(PER_LINUX32);
	current->thread.map_base  = IA32_PAGE_OFFSET/3;
}