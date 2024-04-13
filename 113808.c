onig_scan_env_set_error_string(ScanEnv* env, int ecode ARG_UNUSED,
				UChar* arg, UChar* arg_end)
{
  env->error     = arg;
  env->error_end = arg_end;
}