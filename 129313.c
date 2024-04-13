managesieve_parser_literal_end(struct managesieve_parser *parser)
{
	if ((parser->flags & MANAGESIEVE_PARSE_FLAG_STRING_STREAM) == 0) {
		if (parser->line_size >= parser->max_line_size ||
		    parser->literal_size >
		    	parser->max_line_size - parser->line_size) {
			/* too long string, abort. */
			parser->error = "Literal size too large";
			parser->fatal_error = TRUE;
			return FALSE;
		}
	}

	parser->cur_type = ARG_PARSE_LITERAL_DATA;
	parser->literal_skip_crlf = TRUE;

	parser->cur_pos = 0;
	return TRUE;
}