static bool isStaticResourceUrl(const struct mg_request_info *request_info, u_int len) {
  if((len >= 3 && (!strncmp(&request_info->uri[len - 3], ".js", 3)))
     || (len >= 4 && (!strncmp(&request_info->uri[len - 4], ".css", 4)
		      || !strncmp(&request_info->uri[len - 4], ".map", 4)
		      || !strncmp(&request_info->uri[len - 4], ".ttf", 4)))
     || (len >= 6 && (!strncmp(&request_info->uri[len - 6], ".woff2", 6))))
    return true;

  return false;
}