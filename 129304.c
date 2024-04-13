managesieve_parser_read_literal_data(struct managesieve_parser *parser,
					 const unsigned char *data,
					 size_t data_size)
{
	if (parser->literal_skip_crlf) {

		/* skip \r\n or \n, anything else gives an error */
		if (data_size == 0)
			return FALSE;

		if (*data == '\r') {
			parser->line_size++;
			data++; data_size--;
			i_stream_skip(parser->input, 1);

			if (data_size == 0)
				return FALSE;
		}

		if (*data != '\n') {
			parser->error = "Missing LF after literal size";
			return FALSE;
		}

		parser->line_size++;
		data++; data_size--;
		i_stream_skip(parser->input, 1);

		parser->literal_skip_crlf = FALSE;

		i_assert(parser->cur_pos == 0);
	}

	if ((parser->flags & MANAGESIEVE_PARSE_FLAG_STRING_STREAM) == 0) {
		/* now we just wait until we've read enough data */
		if (data_size < parser->literal_size) {
			return FALSE;
		} else {
			if ( !uni_utf8_data_is_valid
				(data, (size_t)parser->literal_size) ) {
				parser->error = "Invalid UTF-8 character in literal string.";
				return FALSE;
			}

			managesieve_parser_save_arg(parser, data,
					     (size_t)parser->literal_size);
			parser->cur_pos = (size_t)parser->literal_size;
			return TRUE;
		}
	} else {
		/* we don't read the data; we just create a stream for the literal */
		parser->eol = TRUE;
		parser->str_stream = i_stream_create_limit
			(parser->input, parser->literal_size);
		managesieve_parser_save_arg(parser, NULL, 0);
		return TRUE;
	}
}