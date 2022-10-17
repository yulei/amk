

#ifndef AMK_DEBOUNCE
#define AMK_DEBOUNCE   SYM_DEFER_G 
#endif

#define SYM_DEFER_PER_KEY           1
#define SYM_DEFER_PER_ROW           2
#define SYM_EAGER_PER_KEY           3
#define SYM_EAGER_PER_ROW           4
#define ASYM_EAGER_DEFER_PER_KEY    5
#define SYM_DEFER_G                 6

#if AMK_DEBOUNCE == SYM_DEFER_PER_KEY
#include "sym_defer_pk.c"
#elif AMK_DEBOUNCE == SYM_DEFER_PER_ROW
#include "sym_defer_pr.c"
#elif AMK_DEBOUNCE == SYM_EAGER_PER_KEY
#include "sym_defer_pk.c"
#elif AMK_DEBOUNCE == SYM_EAGER_PER_ROW
#include "sym_defer_pk.c"
#elif AMK_DEBOUNCE == ASYM_EAGER_DEFER_PER_KEY
#include "asym_eager_defer_pk.c"
#elif AMK_DEBOUNCE == SYM_DEFER_G
#include "sym_defer_g.c"
#else
#warning "NO debounce algorithm defined"
#include "none.c"
#endif