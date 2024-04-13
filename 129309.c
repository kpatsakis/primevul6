managesieve_parser_create(struct istream *input, size_t max_line_size)
{
	struct managesieve_parser *parser;

	parser = i_new(struct managesieve_parser, 1);
	parser->pool = pool_alloconly_create("MANAGESIEVE parser", 8192);
	parser->input = input;
	parser->max_line_size = max_line_size;

	p_array_init(&parser->root_list, parser->pool, LIST_INIT_COUNT);
	parser->cur_list = &parser->root_list;
	return parser;
}