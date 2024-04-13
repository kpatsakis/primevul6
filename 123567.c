bool ContentSettingsObserver::AllowImage(bool enabled_per_settings,
                                         const WebURL& image_url) {
  if (enabled_per_settings && IsWhitelistedForContentSettings())
    return true;

  bool allow = enabled_per_settings;
  GURL secondary_url(image_url);
  if (content_settings_manager_->content_settings()) {
    allow =
        content_settings_manager_->GetSetting(
            ContentSettingsManager::GetOriginOrURL(
                render_frame()->GetWebFrame()),
            secondary_url,
            "images",
            allow) != CONTENT_SETTING_BLOCK;
  }

  if (!allow)
    DidBlockContentType("images", secondary_url.spec());
  return allow;
}