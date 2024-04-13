static int isCaptiveURL(char *url) {
  if((!strcmp(url, KINDLE_WIFISTUB_URL))
     || (!strcmp(url, HOTSPOT_DETECT_URL))
     || (!strcmp(url, HOTSPOT_DETECT_LUA_URL))
     || (!strcmp(url, ntop->getPrefs()->getCaptivePortalUrl()))
     || (!strcmp(url, AUTHORIZE_CAPTIVE_LUA_URL))
     || (!strcmp(url, "/"))
     )
    return(1);
  else
    return(0);
}