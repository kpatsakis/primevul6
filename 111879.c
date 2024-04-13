flatpak_policy_to_string (FlatpakPolicy policy)
{
  if (policy == FLATPAK_POLICY_SEE)
    return "see";
  if (policy == FLATPAK_POLICY_TALK)
    return "talk";
  if (policy == FLATPAK_POLICY_OWN)
    return "own";

  return "none";
}