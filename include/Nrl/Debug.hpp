#pragma once

#define NRL_PANIC(x) throw x;

#define NRL_VERIFY(x, msg) if(!(x)) NA_PANIC(msg);

#ifdef NRL_DO_ASSERTIONS
    #define NRL_ASSERT(x, msg) NRL_VERIFY(x, msg)
#else
    #define NRL_ASSERT(x, msg)
#endif
