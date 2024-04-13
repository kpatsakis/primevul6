  AstBase(const AstBase &ast, const char *a_original_name,
          size_t a_position = 0, size_t a_length = 0,
          size_t a_original_choice_count = 0, size_t a_original_choise = 0)
      : path(ast.path), line(ast.line), column(ast.column), name(ast.name),
        position(a_position), length(a_length), choice_count(ast.choice_count),
        choice(ast.choice), original_name(a_original_name),
        original_choice_count(a_original_choice_count),
        original_choice(a_original_choise), tag(ast.tag),
        original_tag(str2tag(a_original_name)), is_token(ast.is_token),
        token(ast.token), nodes(ast.nodes), parent(ast.parent) {}