  void moduleInit() override {
    // TODO make these PHP_INI_ALL and thread local once we use them
    IniSetting::Bind(this, IniSetting::PHP_INI_SYSTEM, "mbstring.http_input",
                     &http_input);
    IniSetting::Bind(this, IniSetting::PHP_INI_SYSTEM, "mbstring.http_output",
                     &http_output);
    IniSetting::Bind(this, IniSetting::PHP_INI_ALL,
                     "mbstring.substitute_character",
                     &MBSTRG(current_filter_illegal_mode));

    HHVM_RC_INT(MB_OVERLOAD_MAIL, 1);
    HHVM_RC_INT(MB_OVERLOAD_STRING, 2);
    HHVM_RC_INT(MB_OVERLOAD_REGEX, 4);

    HHVM_RC_INT(MB_CASE_UPPER, PHP_UNICODE_CASE_UPPER);
    HHVM_RC_INT(MB_CASE_LOWER, PHP_UNICODE_CASE_LOWER);
    HHVM_RC_INT(MB_CASE_TITLE, PHP_UNICODE_CASE_TITLE);

    HHVM_FE(mb_list_encodings);
    HHVM_FE(mb_list_encodings_alias_names);
    HHVM_FE(mb_list_mime_names);
    HHVM_FE(mb_check_encoding);
    HHVM_FE(mb_convert_case);
    HHVM_FE(mb_convert_encoding);
    HHVM_FE(mb_convert_kana);
    HHVM_FE(mb_convert_variables);
    HHVM_FE(mb_decode_mimeheader);
    HHVM_FE(mb_decode_numericentity);
    HHVM_FE(mb_detect_encoding);
    HHVM_FE(mb_detect_order);
    HHVM_FE(mb_encode_mimeheader);
    HHVM_FE(mb_encode_numericentity);
    HHVM_FE(mb_encoding_aliases);
    HHVM_FE(mb_ereg_match);
    HHVM_FE(mb_ereg_replace);
    HHVM_FE(mb_ereg_search_getpos);
    HHVM_FE(mb_ereg_search_getregs);
    HHVM_FE(mb_ereg_search_init);
    HHVM_FE(mb_ereg_search_pos);
    HHVM_FE(mb_ereg_search_regs);
    HHVM_FE(mb_ereg_search_setpos);
    HHVM_FE(mb_ereg_search);
    HHVM_FE(mb_ereg);
    HHVM_FE(mb_eregi_replace);
    HHVM_FE(mb_eregi);
    HHVM_FE(mb_get_info);
    HHVM_FE(mb_http_input);
    HHVM_FE(mb_http_output);
    HHVM_FE(mb_internal_encoding);
    HHVM_FE(mb_language);
    HHVM_FE(mb_output_handler);
    HHVM_FE(mb_parse_str);
    HHVM_FE(mb_preferred_mime_name);
    HHVM_FE(mb_regex_encoding);
    HHVM_FE(mb_regex_set_options);
    HHVM_FE(mb_send_mail);
    HHVM_FE(mb_split);
    HHVM_FE(mb_strcut);
    HHVM_FE(mb_strimwidth);
    HHVM_FE(mb_stripos);
    HHVM_FE(mb_stristr);
    HHVM_FE(mb_strlen);
    HHVM_FE(mb_strpos);
    HHVM_FE(mb_strrchr);
    HHVM_FE(mb_strrichr);
    HHVM_FE(mb_strripos);
    HHVM_FE(mb_strrpos);
    HHVM_FE(mb_strstr);
    HHVM_FE(mb_strtolower);
    HHVM_FE(mb_strtoupper);
    HHVM_FE(mb_strwidth);
    HHVM_FE(mb_substitute_character);
    HHVM_FE(mb_substr_count);
    HHVM_FE(mb_substr);

    loadSystemlib();
  }