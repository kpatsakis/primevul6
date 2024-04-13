static void write_pack_info_file(FILE *fp)
{
	int i;
	for (i = 0; i < num_pack; i++)
		fprintf(fp, "P %s\n", info[i]->p->pack_name + objdirlen + 6);
	fputc('\n', fp);
}