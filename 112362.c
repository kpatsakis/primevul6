  void moduleInit() override {
    HHVM_RC_INT_SAME(UCOL_DEFAULT);

    HHVM_RC_INT_SAME(UCOL_PRIMARY);
    HHVM_RC_INT_SAME(UCOL_SECONDARY);
    HHVM_RC_INT_SAME(UCOL_TERTIARY);
    HHVM_RC_INT_SAME(UCOL_DEFAULT_STRENGTH);
    HHVM_RC_INT_SAME(UCOL_QUATERNARY);
    HHVM_RC_INT_SAME(UCOL_IDENTICAL);

    HHVM_RC_INT_SAME(UCOL_OFF);
    HHVM_RC_INT_SAME(UCOL_ON);

    HHVM_RC_INT_SAME(UCOL_SHIFTED);
    HHVM_RC_INT_SAME(UCOL_NON_IGNORABLE);

    HHVM_RC_INT_SAME(UCOL_LOWER_FIRST);
    HHVM_RC_INT_SAME(UCOL_UPPER_FIRST);

    HHVM_RC_INT_SAME(UCOL_FRENCH_COLLATION);
    HHVM_RC_INT_SAME(UCOL_ALTERNATE_HANDLING);
    HHVM_RC_INT_SAME(UCOL_CASE_FIRST);
    HHVM_RC_INT_SAME(UCOL_CASE_LEVEL);
    HHVM_RC_INT_SAME(UCOL_NORMALIZATION_MODE);
    HHVM_RC_INT_SAME(UCOL_STRENGTH);
    HHVM_RC_INT_SAME(UCOL_HIRAGANA_QUATERNARY_MODE);
    HHVM_RC_INT_SAME(UCOL_NUMERIC_COLLATION);

    HHVM_RC_INT(ARRAY_FILTER_USE_BOTH, 1);
    HHVM_RC_INT(ARRAY_FILTER_USE_KEY, 2);

    HHVM_RC_INT(CASE_LOWER,      CaseMode::LOWER);
    HHVM_RC_INT(CASE_UPPER,      CaseMode::UPPER);

    HHVM_RC_INT(COUNT_NORMAL,    CountMode::NORMAL);
    HHVM_RC_INT(COUNT_RECURSIVE, CountMode::RECURSIVE);

    HHVM_RC_INT_SAME(SORT_ASC);
    HHVM_RC_INT_SAME(SORT_DESC);
    HHVM_RC_INT_SAME(SORT_FLAG_CASE);
    HHVM_RC_INT_SAME(SORT_LOCALE_STRING);
    HHVM_RC_INT_SAME(SORT_NATURAL);
    HHVM_RC_INT_SAME(SORT_NUMERIC);
    HHVM_RC_INT_SAME(SORT_REGULAR);
    HHVM_RC_INT_SAME(SORT_STRING);

    HHVM_FE(array_change_key_case);
    HHVM_FE(array_chunk);
    HHVM_FE(array_column);
    HHVM_FE(array_combine);
    HHVM_FE(array_count_values);
    HHVM_FE(array_fill_keys);
    HHVM_FE(array_fill);
    HHVM_FE(array_flip);
    HHVM_FE(array_key_exists);
    HHVM_FE(key_exists);
    HHVM_FE(array_keys);
    HHVM_FALIAS(__SystemLib\\array_map, array_map);
    HHVM_FE(array_merge_recursive);
    HHVM_FE(array_merge);
    HHVM_FE(array_replace_recursive);
    HHVM_FE(array_replace);
    HHVM_FE(array_pad);
    HHVM_FE(array_pop);
    HHVM_FE(array_product);
    HHVM_FE(array_push);
    HHVM_FE(array_rand);
    HHVM_FE(array_reverse);
    HHVM_FE(array_search);
    HHVM_FE(array_shift);
    HHVM_FE(array_slice);
    HHVM_FE(array_splice);
    HHVM_FE(array_sum);
    HHVM_FE(array_unique);
    HHVM_FE(array_unshift);
    HHVM_FE(array_values);
    HHVM_FE(array_walk_recursive);
    HHVM_FE(array_walk);
    HHVM_FE(compact);
    HHVM_FALIAS(__SystemLib\\compact_sl, __SystemLib_compact_sl);
    HHVM_FE(shuffle);
    HHVM_FE(count);
    HHVM_FE(sizeof);
    HHVM_FE(each);
    HHVM_FE(current);
    HHVM_FE(next);
    HHVM_FE(pos);
    HHVM_FE(prev);
    HHVM_FE(reset);
    HHVM_FE(end);
    HHVM_FE(key);
    HHVM_FE(in_array);
    HHVM_FE(range);
    HHVM_FE(array_diff);
    HHVM_FE(array_udiff);
    HHVM_FE(array_diff_assoc);
    HHVM_FE(array_diff_uassoc);
    HHVM_FE(array_udiff_assoc);
    HHVM_FE(array_udiff_uassoc);
    HHVM_FE(array_diff_key);
    HHVM_FE(array_diff_ukey);
    HHVM_FE(array_intersect);
    HHVM_FE(array_uintersect);
    HHVM_FE(array_intersect_assoc);
    HHVM_FE(array_intersect_uassoc);
    HHVM_FE(array_uintersect_assoc);
    HHVM_FE(array_uintersect_uassoc);
    HHVM_FE(array_intersect_key);
    HHVM_FE(array_intersect_ukey);
    HHVM_FE(sort);
    HHVM_FE(rsort);
    HHVM_FE(asort);
    HHVM_FE(arsort);
    HHVM_FE(ksort);
    HHVM_FE(krsort);
    HHVM_FE(usort);
    HHVM_FE(uasort);
    HHVM_FE(uksort);
    HHVM_FE(natsort);
    HHVM_FE(natcasesort);
    HHVM_FE(i18n_loc_get_default);
    HHVM_FE(i18n_loc_set_default);
    HHVM_FE(i18n_loc_set_attribute);
    HHVM_FE(i18n_loc_set_strength);
    HHVM_FE(i18n_loc_get_error_code);
    HHVM_FE(hphp_array_idx);
    HHVM_FE(array_multisort);
    HHVM_FALIAS(HH\\dict, HH_dict);
    HHVM_FALIAS(HH\\vec, HH_vec);

    loadSystemlib();
  }