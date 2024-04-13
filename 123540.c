bool ContentSettingsObserver::AllowScriptFromSource(
    bool enabled_per_settings,
    const blink::WebURL& script_url) {
  if (IsWhitelistedForContentSettings())
    return true;

  bool allow = enabled_per_settings;
  GURL secondary_url(script_url);
  if (content_settings_manager_->content_settings()) {
    allow =
        content_settings_manager_->GetSetting(
          ContentSettingsManager::GetOriginOrURL(render_frame()->GetWebFrame()),
          secondary_url,
          "javascript",
          allow) != CONTENT_SETTING_BLOCK;
  }

  allow = allow || IsWhitelistedForContentSettings();
  if (!allow)
    DidBlockContentType("javascript", secondary_url.spec());
  return allow;
}