static int isWhitelistedURI(char *uri) {
  /* URL whitelist */
  if((!strcmp(uri,    LOGIN_URL))
     || (!strcmp(uri, AUTHORIZE_URL))
     || (!strcmp(uri, BANNED_SITE_URL))
     || (!strcmp(uri, PLEASE_WAIT_URL))
     || (!strcmp(uri, HOTSPOT_DETECT_URL))
     || (!strcmp(uri, HOTSPOT_DETECT_LUA_URL))
     || (!strcmp(uri, ntop->getPrefs()->getCaptivePortalUrl()))
     || (!strcmp(uri, KINDLE_WIFISTUB_URL))
     )
    return(1);
  else
    return(0);
}