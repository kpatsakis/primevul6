int main(int argc, char* argv[])
{
	size_t elements = sizeof(args) / sizeof(args[0]);
	size_t x;
	const char* fname = "xfreerdp-argument.1.xml";
	FILE* fp = NULL;
	/* Open output file for writing, truncate if existing. */
	fp = fopen(fname, "w");

	if (NULL == fp)
	{
		fprintf(stderr,  "Could not open '%s' for writing.\n", fname);
		return -1;
	}

	/* The tag used as header in the manpage */
	fprintf(fp, "<refsect1>\n");
	fprintf(fp, "\t<title>Options</title>\n");
	fprintf(fp, "\t\t<variablelist>\n");

	/* Iterate over argument struct and write data to docbook 4.5
	 * compatible XML */
	if (elements < 2)
	{
		fprintf(stderr,  "The argument array 'args' is empty, writing an empty file.\n");
		elements = 1;
	}

	for (x = 0; x < elements - 1; x++)
	{
		const COMMAND_LINE_ARGUMENT_A* arg = &args[x];
		char* name = tr_esc_str((LPSTR) arg->Name, FALSE);
		char* alias = tr_esc_str((LPSTR) arg->Alias, FALSE);
		char* format = tr_esc_str(arg->Format, TRUE);
		char* text = tr_esc_str((LPSTR) arg->Text, FALSE);
		fprintf(fp, "\t\t\t<varlistentry>\n");

		do
		{
			fprintf(fp, "\t\t\t\t<term><option>");

			if (arg->Flags == COMMAND_LINE_VALUE_BOOL)
				fprintf(fp, "%s", arg->Default ? "-" : "+");
			else
				fprintf(fp, "/");

			fprintf(fp, "%s</option>", name);

			if (format)
			{
				if (arg->Flags == COMMAND_LINE_VALUE_OPTIONAL)
					fprintf(fp, "[");

				fprintf(fp, ":%s", format);

				if (arg->Flags == COMMAND_LINE_VALUE_OPTIONAL)
					fprintf(fp, "]");
			}

			fprintf(fp, "</term>\n");

			if (alias == name)
				break;

			free(name);
			name = alias;
		}
		while (alias);

		if (text)
		{
			fprintf(fp, "\t\t\t\t<listitem>\n");
			fprintf(fp, "\t\t\t\t\t<para>");

			if (text)
				fprintf(fp, "%s", text);

			if (arg->Flags == COMMAND_LINE_VALUE_BOOL)
				fprintf(fp, " (default:%s)", arg->Default ? "on" : "off");
			else if (arg->Default)
			{
				char* value = tr_esc_str((LPSTR) arg->Default, FALSE);
				fprintf(fp, " (default:%s)", value);
				free(value);
			}

			fprintf(fp, "</para>\n");
			fprintf(fp, "\t\t\t\t</listitem>\n");
		}

		fprintf(fp, "\t\t\t</varlistentry>\n");
		free(name);
		free(format);
		free(text);
	}

	fprintf(fp, "\t\t</variablelist>\n");
	fprintf(fp, "\t</refsect1>\n");
	fclose(fp);
	return 0;
}