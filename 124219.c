const char *get_secure_cookie_attributes(const struct mg_request_info *request_info) {
  if(request_info->is_ssl)
    return " HttpOnly; SameSite=lax; Secure";
  else
    return " HttpOnly; SameSite=lax";
}