managesieve_parser_read_arg(struct managesieve_parser *parser)
{
	const unsigned char *data;
	size_t data_size;

	data = i_stream_get_data(parser->input, &data_size);
	if (data_size == 0)
		return FALSE;

	while (parser->cur_type == ARG_PARSE_NONE) {
		/* we haven't started parsing yet */
		if (!managesieve_parser_skip_to_next(parser, &data, &data_size))
			return FALSE;
		i_assert(parser->cur_pos == 0);

		switch (data[0]) {
		case '\r':
		case '\n':
			/* unexpected end of line */
			parser->eol = TRUE;
			return FALSE;
		case '"':
			parser->cur_type = ARG_PARSE_STRING;
			parser->str_first_escape = -1;
			break;
		case '{':
			parser->cur_type = ARG_PARSE_LITERAL;
			parser->literal_size = 0;
			parser->literal_nonsync = FALSE;
			break;
		default:
			if (!is_valid_atom_char(parser, data[0]))
				return FALSE;
			parser->cur_type = ARG_PARSE_ATOM;
			break;
		}

		parser->cur_pos++;
	}

	i_assert(data_size > 0);

	switch (parser->cur_type) {
	case ARG_PARSE_ATOM:
		if (!managesieve_parser_read_atom(parser, data, data_size))
			return FALSE;
		break;
	case ARG_PARSE_STRING:
		if ((parser->flags & MANAGESIEVE_PARSE_FLAG_STRING_STREAM) != 0) {
			parser->eol = TRUE;
			parser->line_size += parser->cur_pos;
			i_stream_skip(parser->input, parser->cur_pos);
			parser->cur_pos = 0;
			parser->str_stream = quoted_string_istream_create(parser);
			managesieve_parser_save_arg(parser, NULL, 0);

		} else if (!managesieve_parser_read_string(parser, data, data_size)) {
			return FALSE;
		}
		break;
	case ARG_PARSE_LITERAL:
		if (!managesieve_parser_read_literal(parser, data, data_size))
			return FALSE;

		/* pass through to parsing data. since input->skip was
		   modified, we need to get the data start position again. */
		data = i_stream_get_data(parser->input, &data_size);

		/* fall through */
	case ARG_PARSE_LITERAL_DATA:
		if (!managesieve_parser_read_literal_data(parser, data, data_size))
			return FALSE;
		break;
	default:
		i_unreached();
	}

	i_assert(parser->cur_type == ARG_PARSE_NONE);
	return TRUE;
}