bool ContentSettingsObserver::AllowRunningInsecureContent(
    bool allowed_per_settings,
    const blink::WebSecurityOrigin& origin,
    const blink::WebURL& resource_url) {
  // TODO(bridiver) is origin different than web frame top origin?
  bool allow = allowed_per_settings;
  GURL secondary_url(resource_url);
  if (content_settings_manager_->content_settings()) {
    allow =
        content_settings_manager_->GetSetting(
            ContentSettingsManager::GetOriginOrURL(
                render_frame()->GetWebFrame()),
            secondary_url,
            "runInsecureContent",
            allow) != CONTENT_SETTING_BLOCK;
  }

  if (allow)
    DidRunInsecureContent(GURL(resource_url));
  else
    DidBlockRunInsecureContent(GURL(resource_url));
  return allow;
}