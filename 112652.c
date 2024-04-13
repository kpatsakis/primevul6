static int php_mail(const char *to, const char *subject, const char *message,
                    const char *headers, const char *extra_cmd) {
  const char *sendmail_path = "/usr/sbin/sendmail -t -i";
  String sendmail_cmd = sendmail_path;
  if (extra_cmd != nullptr) {
    sendmail_cmd += " ";
    sendmail_cmd += extra_cmd;
  }

  /* Since popen() doesn't indicate if the internal fork() doesn't work
   * (e.g. the shell can't be executed) we explicitly set it to 0 to be
   * sure we don't catch any older errno value. */
  errno = 0;
  FILE *sendmail = popen(sendmail_cmd.data(), "w");
  if (sendmail == nullptr) {
    raise_warning("Could not execute mail delivery program '%s'",
                    sendmail_path);
    return 0;
  }

  if (EACCES == errno) {
    raise_warning("Permission denied: unable to execute shell to run "
                    "mail delivery binary '%s'", sendmail_path);
    pclose(sendmail);
    return 0;
  }

  fprintf(sendmail, "To: %s\n", to);
  fprintf(sendmail, "Subject: %s\n", subject);
  if (headers != nullptr) {
    fprintf(sendmail, "%s\n", headers);
  }
  fprintf(sendmail, "\n%s\n", message);
  int ret = pclose(sendmail);
#if defined(EX_TEMPFAIL)
  if ((ret != EX_OK) && (ret != EX_TEMPFAIL)) return 0;
#elif defined(EX_OK)
  if (ret != EX_OK) return 0;
#else
  if (ret != 0) return 0;
#endif
  return 1;
}