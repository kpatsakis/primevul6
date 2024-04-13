usage (void)
{
  g_print ("%s\n", _("Usage: exo-open [URLs...]"));
  g_print ("%s\n", _("       exo-open --launch TYPE [PARAMETERs...]"));
  g_print ("\n");
  g_print ("%s\n", _("  -?, --help                          Print this help message and exit"));
  g_print ("%s\n", _("  -V, --version                       Print version information and exit"));
  g_print ("\n");
  g_print ("%s\n", _("  --launch TYPE [PARAMETERs...]       Launch the preferred application of\n"
                     "                                      TYPE with the optional PARAMETERs, where\n"
                     "                                      TYPE is one of the following values."));
  g_print ("\n");
  g_print ("%s\n", _("  --working-directory DIRECTORY       Default working directory for applications\n"
                     "                                      when using the --launch option."));
  g_print ("\n");
  g_print ("%s\n", _("The following TYPEs are supported for the --launch command:"));
  g_print ("\n");

  /* Note to Translators: Do not translate the TYPEs (WebBrowser, MailReader, TerminalEmulator),
   * since the xfce4-mime-helper utility will not accept localized TYPEs.
   */
  g_print ("%s\n", _("  WebBrowser       - The preferred Web Browser.\n"
                     "  MailReader       - The preferred Mail Reader.\n"
                     "  FileManager      - The preferred File Manager.\n"
                     "  TerminalEmulator - The preferred Terminal Emulator."));
  g_print ("\n");
  g_print ("%s\n", _("If you don't specify the --launch option, exo-open will open all specified\n"
                     "URLs with their preferred URL handlers. Else, if you specify the --launch\n"
                     "option, you can select which preferred application you want to run, and\n"
                     "pass additional parameters to the application (i.e. for TerminalEmulator\n"
                     "you can pass the command line that should be run in the terminal)."));
  g_print ("\n");
}