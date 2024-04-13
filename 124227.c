static void generate_session_id(char *buf, const char *random, const char *user) {
  mg_md5(buf, random, user, NULL);
}