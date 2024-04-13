  void setup_actions() {
    g["Definition"] = [&](const SemanticValues &sv, any &dt) {
      Data &data = *any_cast<Data *>(dt);

      auto is_macro = sv.choice() == 0;
      auto ignore = any_cast<bool>(sv[0]);
      auto name = any_cast<std::string>(sv[1]);

      std::vector<std::string> params;
      std::shared_ptr<Ope> ope;
      if (is_macro) {
        params = any_cast<std::vector<std::string>>(sv[2]);
        ope = any_cast<std::shared_ptr<Ope>>(sv[4]);
        if (sv.size() == 6) {
          data.instructions[name] = any_cast<Instruction>(sv[5]);
        }
      } else {
        ope = any_cast<std::shared_ptr<Ope>>(sv[3]);
        if (sv.size() == 5) {
          data.instructions[name] = any_cast<Instruction>(sv[4]);
        }
      }

      auto &grammar = *data.grammar;
      if (!grammar.count(name)) {
        auto &rule = grammar[name];
        rule <= ope;
        rule.name = name;
        rule.s_ = sv.c_str();
        rule.ignoreSemanticValue = ignore;
        rule.is_macro = is_macro;
        rule.params = params;

        if (data.start.empty()) {
          data.start = name;
          data.start_pos = sv.c_str();
        }
      } else {
        data.duplicates.emplace_back(name, sv.c_str());
      }
    };

    g["Expression"] = [&](const SemanticValues &sv) {
      if (sv.size() == 1) {
        return any_cast<std::shared_ptr<Ope>>(sv[0]);
      } else {
        std::vector<std::shared_ptr<Ope>> opes;
        for (auto i = 0u; i < sv.size(); i++) {
          opes.emplace_back(any_cast<std::shared_ptr<Ope>>(sv[i]));
        }
        const std::shared_ptr<Ope> ope =
            std::make_shared<PrioritizedChoice>(opes);
        return ope;
      }
    };

    g["Sequence"] = [&](const SemanticValues &sv) {
      if (sv.size() == 1) {
        return any_cast<std::shared_ptr<Ope>>(sv[0]);
      } else {
        std::vector<std::shared_ptr<Ope>> opes;
        for (const auto &x : sv) {
          opes.emplace_back(any_cast<std::shared_ptr<Ope>>(x));
        }
        const std::shared_ptr<Ope> ope = std::make_shared<Sequence>(opes);
        return ope;
      }
    };

    g["Prefix"] = [&](const SemanticValues &sv) {
      std::shared_ptr<Ope> ope;
      if (sv.size() == 1) {
        ope = any_cast<std::shared_ptr<Ope>>(sv[0]);
      } else {
        assert(sv.size() == 2);
        auto tok = any_cast<char>(sv[0]);
        ope = any_cast<std::shared_ptr<Ope>>(sv[1]);
        if (tok == '&') {
          ope = apd(ope);
        } else { // '!'
          ope = npd(ope);
        }
      }
      return ope;
    };

    struct Loop {
      enum class Type { opt = 0, zom, oom, rep };
      Type type;
      std::pair<size_t, size_t> range;
    };

    g["Suffix"] = [&](const SemanticValues &sv) {
      auto ope = any_cast<std::shared_ptr<Ope>>(sv[0]);
      if (sv.size() == 1) {
        return ope;
      } else {
        assert(sv.size() == 2);
        auto loop = any_cast<Loop>(sv[1]);
        switch (loop.type) {
        case Loop::Type::opt: return opt(ope);
        case Loop::Type::zom: return zom(ope);
        case Loop::Type::oom: return oom(ope);
        default: // Regex-like repetition
          return rep(ope, loop.range.first, loop.range.second);
        }
      }
    };

    g["Loop"] = [&](const SemanticValues &sv) {
      switch (sv.choice()) {
      case 0: // Option
        return Loop{Loop::Type::opt, std::pair<size_t, size_t>()};
      case 1: // Zero or More
        return Loop{Loop::Type::zom, std::pair<size_t, size_t>()};
      case 2: // One or More
        return Loop{Loop::Type::oom, std::pair<size_t, size_t>()};
      default: // Regex-like repetition
        return Loop{Loop::Type::rep,
                    any_cast<std::pair<size_t, size_t>>(sv[0])};
      }
    };

    g["RepetitionRange"] = [&](const SemanticValues &sv) {
      switch (sv.choice()) {
      case 0: { // Number COMMA Number
        auto min = any_cast<size_t>(sv[0]);
        auto max = any_cast<size_t>(sv[1]);
        return std::make_pair(min, max);
      }
      case 1: // Number COMMA
        return std::make_pair(any_cast<size_t>(sv[0]),
                              std::numeric_limits<size_t>::max());
      case 2: { // Number
        auto n = any_cast<size_t>(sv[0]);
        return std::make_pair(n, n);
      }
      default: // COMMA Number
        return std::make_pair(std::numeric_limits<size_t>::min(),
                              any_cast<size_t>(sv[0]));
      }
    };
    g["Number"] = [&](const SemanticValues &sv) {
      std::stringstream ss(sv.str());
      size_t n;
      ss >> n;
      return n;
    };

    g["Primary"] = [&](const SemanticValues &sv, any &dt) {
      Data &data = *any_cast<Data *>(dt);

      switch (sv.choice()) {
      case 0:   // Macro Reference
      case 1: { // Reference
        auto is_macro = sv.choice() == 0;
        auto ignore = any_cast<bool>(sv[0]);
        const auto &ident = any_cast<std::string>(sv[1]);

        std::vector<std::shared_ptr<Ope>> args;
        if (is_macro) {
          args = any_cast<std::vector<std::shared_ptr<Ope>>>(sv[2]);
        }

        std::shared_ptr<Ope> ope =
            ref(*data.grammar, ident, sv.c_str(), is_macro, args);

        if (ignore) {
          return ign(ope);
        } else {
          return ope;
        }
      }
      case 2: { // (Expression)
        return any_cast<std::shared_ptr<Ope>>(sv[0]);
      }
      case 3: { // TokenBoundary
        return tok(any_cast<std::shared_ptr<Ope>>(sv[0]));
      }
      case 4: { // CaptureScope
        return csc(any_cast<std::shared_ptr<Ope>>(sv[0]));
      }
      case 5: { // Capture
        const auto &name = any_cast<std::string>(sv[0]);
        auto ope = any_cast<std::shared_ptr<Ope>>(sv[1]);
        return cap(ope, [name](const char *a_s, size_t a_n, Context &c) {
          auto &cs = c.capture_scope_stack[c.capture_scope_stack_size - 1];
          cs[name] = std::string(a_s, a_n);
        });
      }
      default: {
        return any_cast<std::shared_ptr<Ope>>(sv[0]);
      }
      }
    };

    g["IdentCont"] = [](const SemanticValues &sv) {
      return std::string(sv.c_str(), sv.length());
    };

    g["Dictionary"] = [](const SemanticValues &sv) {
      auto items = sv.transform<std::string>();
      return dic(items);
    };

    g["Literal"] = [](const SemanticValues &sv) {
      const auto &tok = sv.tokens.front();
      return lit(resolve_escape_sequence(tok.first, tok.second));
    };
    g["LiteralI"] = [](const SemanticValues &sv) {
      const auto &tok = sv.tokens.front();
      return liti(resolve_escape_sequence(tok.first, tok.second));
    };
    g["LiteralD"] = [](const SemanticValues &sv) {
      auto &tok = sv.tokens.front();
      return resolve_escape_sequence(tok.first, tok.second);
    };

    g["Class"] = [](const SemanticValues &sv) {
      auto ranges = sv.transform<std::pair<char32_t, char32_t>>();
      return cls(ranges);
    };
    g["NegatedClass"] = [](const SemanticValues &sv) {
      auto ranges = sv.transform<std::pair<char32_t, char32_t>>();
      return ncls(ranges);
    };
    g["Range"] = [](const SemanticValues &sv) {
      switch (sv.choice()) {
      case 0: {
        auto s1 = any_cast<std::string>(sv[0]);
        auto s2 = any_cast<std::string>(sv[1]);
        auto cp1 = decode_codepoint(s1.c_str(), s1.length());
        auto cp2 = decode_codepoint(s2.c_str(), s2.length());
        return std::make_pair(cp1, cp2);
      }
      case 1: {
        auto s = any_cast<std::string>(sv[0]);
        auto cp = decode_codepoint(s.c_str(), s.length());
        return std::make_pair(cp, cp);
      }
      }
      return std::make_pair<char32_t, char32_t>(0, 0);
    };
    g["Char"] = [](const SemanticValues &sv) {
      return resolve_escape_sequence(sv.c_str(), sv.length());
    };

    g["AND"] = [](const SemanticValues &sv) { return *sv.c_str(); };
    g["NOT"] = [](const SemanticValues &sv) { return *sv.c_str(); };
    g["QUESTION"] = [](const SemanticValues &sv) { return *sv.c_str(); };
    g["STAR"] = [](const SemanticValues &sv) { return *sv.c_str(); };
    g["PLUS"] = [](const SemanticValues &sv) { return *sv.c_str(); };

    g["DOT"] = [](const SemanticValues & /*sv*/) { return dot(); };

    g["BeginCap"] = [](const SemanticValues &sv) { return sv.token(); };

    g["BackRef"] = [&](const SemanticValues &sv) { return bkr(sv.token()); };

    g["Ignore"] = [](const SemanticValues &sv) { return sv.size() > 0; };

    g["Parameters"] = [](const SemanticValues &sv) {
      return sv.transform<std::string>();
    };

    g["Arguments"] = [](const SemanticValues &sv) {
      return sv.transform<std::shared_ptr<Ope>>();
    };

    g["PrecedenceClimbing"] = [](const SemanticValues &sv) {
      PrecedenceClimbing::BinOpeInfo binOpeInfo;
      size_t level = 1;
      for (auto v : sv) {
        auto tokens = any_cast<std::vector<std::string>>(v);
        auto assoc = tokens[0][0];
        for (size_t i = 1; i < tokens.size(); i++) {
          const auto &tok = tokens[i];
          binOpeInfo[tok] = std::make_pair(level, assoc);
        }
        level++;
      }
      Instruction instruction;
      instruction.type = "precedence";
      instruction.data = binOpeInfo;
      return instruction;
    };
    g["PrecedenceInfo"] = [](const SemanticValues &sv) {
      return sv.transform<std::string>();
    };
    g["PrecedenceOpe"] = [](const SemanticValues &sv) { return sv.token(); };
    g["PrecedenceAssoc"] = [](const SemanticValues &sv) { return sv.token(); };
  }