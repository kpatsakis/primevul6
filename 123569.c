bool ContentSettingsObserver::AllowMutationEvents(bool default_value) {
  if (IsWhitelistedForContentSettings())
    return true;

  bool allow = default_value;
  if (content_settings_manager_->content_settings()) {
    allow =
        content_settings_manager_->GetSetting(
            ContentSettingsManager::GetOriginOrURL(
                render_frame()->GetWebFrame()),
            GURL(),
            "mutation",
            allow) != CONTENT_SETTING_BLOCK;
  }

  if (!allow)
    DidBlockContentType("mutation");
  return allow;
}