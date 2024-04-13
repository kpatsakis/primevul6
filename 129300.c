is_valid_atom_char(struct managesieve_parser *parser, char chr)
{
	if (IS_ATOM_SPECIAL((unsigned char)chr)) {
		parser->error = "Invalid characters in atom";
		return FALSE;
	} else if ((chr & 0x80) != 0) {
		parser->error = "8bit data in atom";
		return FALSE;
	}

	return TRUE;
}