  MBGlobals() :
    language(mbfl_no_language_uni),
    current_language(mbfl_no_language_uni),
    internal_encoding((mbfl_encoding*) mbfl_no2encoding(mbfl_no_encoding_utf8)),
    current_internal_encoding(internal_encoding),
    http_output_encoding((mbfl_encoding*) &mbfl_encoding_pass),
    current_http_output_encoding((mbfl_encoding*) &mbfl_encoding_pass),
    http_input_identify(nullptr),
    http_input_identify_get(nullptr),
    http_input_identify_post(nullptr),
    http_input_identify_cookie(nullptr),
    http_input_identify_string(nullptr),
    http_input_list(nullptr),
    http_input_list_size(0),
    detect_order_list(nullptr),
    detect_order_list_size(0),
    current_detect_order_list(nullptr),
    current_detect_order_list_size(0),
    default_detect_order_list
    ((mbfl_no_encoding *)php_mb_default_identify_list_neut),
    default_detect_order_list_size
    (sizeof(php_mb_default_identify_list_neut) /
     sizeof(php_mb_default_identify_list_neut[0])),
    filter_illegal_mode(MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR),
    filter_illegal_substchar(0x3f), /* '?' */
    current_filter_illegal_mode(MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR),
    current_filter_illegal_substchar(0x3f), /* '?' */
    encoding_translation(0),
    strict_detection(0),
    illegalchars(0),
    outconv(nullptr),
    default_mbctype(ONIG_ENCODING_UTF8),
    current_mbctype(ONIG_ENCODING_UTF8),
    search_pos(0),
    search_re((php_mb_regex_t*)nullptr),
    search_regs((OnigRegion*)nullptr),
    regex_default_options(ONIG_OPTION_MULTILINE | ONIG_OPTION_SINGLELINE),
    regex_default_syntax(ONIG_SYNTAX_RUBY) {
  }