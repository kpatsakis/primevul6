static int is_elf_fdpic(struct elfhdr *hdr, struct file *file)
{
	if (memcmp(hdr->e_ident, ELFMAG, SELFMAG) != 0)
		return 0;
	if (hdr->e_type != ET_EXEC && hdr->e_type != ET_DYN)
		return 0;
	if (!elf_check_arch(hdr) || !elf_check_fdpic(hdr))
		return 0;
	if (!file->f_op || !file->f_op->mmap)
		return 0;
	return 1;
}