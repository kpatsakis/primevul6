ipa_cp_c_finalize (void)
{
  max_count = profile_count::uninitialized ();
  overall_size = 0;
  max_new_size = 0;
  ipcp_free_transformation_sum ();
}