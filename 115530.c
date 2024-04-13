  AstBase(const char *a_path, size_t a_line, size_t a_column,
          const char *a_name,
          const std::vector<std::shared_ptr<AstBase>> &a_nodes,
          size_t a_position = 0, size_t a_length = 0, size_t a_choice_count = 0,
          size_t a_choice = 0)
      : path(a_path ? a_path : ""), line(a_line), column(a_column),
        name(a_name), position(a_position), length(a_length),
        choice_count(a_choice_count), choice(a_choice), original_name(a_name),
        original_choice_count(a_choice_count), original_choice(a_choice),
        tag(str2tag(a_name)), original_tag(tag), is_token(false),
        nodes(a_nodes) {}