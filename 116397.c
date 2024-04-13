init_scriptfonts(void)
{
  if (*cfg.font_choice) {
    char * cfg_scriptfonts = cs__wcstombs(cfg.font_choice);
    cfg_apply(cfg_scriptfonts, 0);
    free(cfg_scriptfonts);
    use_blockfonts = wcschr(cfg.font_choice, '|');
  }
  scriptfonts_init = true;
}