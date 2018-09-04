// multiple includes are allowed for the trick to work
#ifdef APPLY_TRICKERY
#undef LIB_SYREM_EXPORT
#define LIB_SYREM_EXPORT
#else
#define APPLY_TRICKERY
#endif
