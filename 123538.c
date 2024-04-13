void ContentSettingsObserver::DidNotAllowPlugins() {
  DidBlockContentType(CONTENT_SETTINGS_TYPE_PLUGINS);
}