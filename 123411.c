AP4_HvccAtom::GetProfileName(AP4_UI08 profile_space, AP4_UI08 profile)
{
    if (profile_space != 0) {
        return NULL;
    }
    switch (profile) {
        case AP4_HEVC_PROFILE_MAIN:               return "Main";
        case AP4_HEVC_PROFILE_MAIN_10:            return "Main 10";
        case AP4_HEVC_PROFILE_MAIN_STILL_PICTURE: return "Main Still Picture";
        case AP4_HEVC_PROFILE_REXT:               return "Rext";
    }

    return NULL;
}