  Reference(const Grammar &grammar, const std::string &name, const char *s,
            bool is_macro, const std::vector<std::shared_ptr<Ope>> &args)
      : grammar_(grammar), name_(name), s_(s), is_macro_(is_macro), args_(args),
        rule_(nullptr), iarg_(0) {}