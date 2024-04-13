AP4_AvccAtom::GetProfileName(AP4_UI08 profile)
{
    switch (profile) {
        case AP4_AVC_PROFILE_BASELINE: return "Baseline";   
        case AP4_AVC_PROFILE_MAIN:     return "Main";
        case AP4_AVC_PROFILE_EXTENDED: return "Extended";
        case AP4_AVC_PROFILE_HIGH:     return "High";
        case AP4_AVC_PROFILE_HIGH_10:  return "High 10";
        case AP4_AVC_PROFILE_HIGH_422: return "High 4:2:2";
        case AP4_AVC_PROFILE_HIGH_444: return "High 4:4:4";
    }

    return NULL;
}