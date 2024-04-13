static int writenote(struct memelfnote *men, struct file *file,
			loff_t *foffset)
{
	struct elf_note en;
	en.n_namesz = strlen(men->name) + 1;
	en.n_descsz = men->datasz;
	en.n_type = men->type;

	DUMP_WRITE(&en, sizeof(en), foffset);
	DUMP_WRITE(men->name, en.n_namesz, foffset);
	if (!alignfile(file, foffset))
		return 0;
	DUMP_WRITE(men->data, men->datasz, foffset);
	if (!alignfile(file, foffset))
		return 0;

	return 1;
}