static int php_mb_nls_get_default_detect_order_list(mbfl_no_language lang,
                                                    mbfl_no_encoding **plist,
                                                    int* plist_size) {
  size_t i;
  *plist = (mbfl_no_encoding *) php_mb_default_identify_list_neut;
  *plist_size = sizeof(php_mb_default_identify_list_neut) /
    sizeof(php_mb_default_identify_list_neut[0]);

  for (i = 0; i < sizeof(php_mb_default_identify_list) /
         sizeof(php_mb_default_identify_list[0]); i++) {
    if (php_mb_default_identify_list[i].lang == lang) {
      *plist = php_mb_default_identify_list[i].list;
      *plist_size = php_mb_default_identify_list[i].list_size;
      return 1;
    }
  }
  return 0;
}