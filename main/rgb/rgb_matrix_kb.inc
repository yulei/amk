#if defined(OPENRGB_ENABLE)
#include "openrgb_direct_anim.h"
#elif defined(SIGNALRGB_ENABLE)
#include "signalrgb_anim.h"
#else
#error "NO custom rgb matrix effect included"
#endif