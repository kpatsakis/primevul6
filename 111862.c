flatpak_filesystem_key_in_home (const char *filesystem)
{
  /* "home" is definitely in home */
  if (strcmp (filesystem, "home") == 0)
    return TRUE;

  /* All the other special fs:es are non-home.
   * Note: This considers absolute paths that are in the homedir as non-home.
   */
  if (g_strv_contains (flatpak_context_special_filesystems, filesystem) ||
      g_str_has_prefix (filesystem, "/"))
    return FALSE;

  /* Files in xdg-run are not in home */
  if (g_str_has_prefix (filesystem, "xdg-run"))
    return FALSE;

  /* All remaining keys (~/, xdg-data, etc) are considered in home,
   * Note: technically $XDG_HOME_DATA could point outside the homedir, but we ignore that.
   */
  return TRUE;
}