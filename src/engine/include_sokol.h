#ifndef INCLUDE_SOKOL_H
#define INCLUDE_SOKOL_H

#undef SOKOL_IMPL

#ifndef SOKOL_GFX_INCLUDED
    #undef SOKOL_GFX_IMPL
    #include <sokol/sokol_gfx.h>
#endif

#ifndef SOKOL_APP_INCLUDED
    #undef SOKOL_APP_IMPL
    #include <sokol/sokol_app.h>
#endif

#ifndef SOKOL_GLUE_INCLUDED
    #undef SOKOL_GLUE_IMPL
    #include <sokol/sokol_glue.h>
#endif

#ifndef SOKOL_LOG_INCLUDED
    #undef SOKOL_LOG_IMPL
    #include <sokol/sokol_log.h>
#endif

#ifndef SOKOL_TIME_INCLUDED
    #undef SOKOL_TIME_IMPL
    #include <sokol/sokol_time.h>
#endif

#endif
