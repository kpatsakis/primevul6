void ContentSettingsObserver::DidBlockContentType(
    ContentSettingsType settings_type) {
  DidBlockContentType(settings_type, base::string16());
}