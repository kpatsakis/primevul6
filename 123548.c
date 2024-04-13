void ContentSettingsObserver::OnLoadBlockedPlugins(
    const std::string& identifier) {
  temporarily_allowed_plugins_.insert(identifier);
}