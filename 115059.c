hint_time_bonus (ipa_hints hints)
{
  int result = 0;
  if (hints & (INLINE_HINT_loop_iterations | INLINE_HINT_loop_stride))
    result += param_ipa_cp_loop_hint_bonus;
  return result;
}