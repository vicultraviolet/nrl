#pragma once

#define NRL_PANIC(x) throw Nrl::Panic::New(x)

#define NRL_VERIFY(x, msg) if(!(x)) NA_PANIC(msg)

#ifdef NRL_DO_ASSERTIONS
    #define NRL_ASSERT(x, msg) NRL_VERIFY(x, msg)
#else
    #define NRL_ASSERT(x, msg)
#endif

namespace Nrl {
    struct Panic {
        [[nodiscard]] static Panic New(const char* what) { return Panic {.what = what}; }
        const char* what = "";
    };
} // namespace Nrl
