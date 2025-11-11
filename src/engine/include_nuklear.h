#ifndef INCLUDE_NUKLEAR_H
#define INCLUDE_NUKLEAR_H

#ifndef NK_NUKLEAR_H_
    #ifndef SOKOL_GFX_INCLUDED
        #undef SOKOL_GFX_IMPL
        #include <sokol/sokol_gfx.h>
    #endif

    #ifndef SOKOL_APP_INCLUDED
        #undef SOKOL_APP_IMPL
        #include <sokol/sokol_app.h>
    #endif

    #undef NK_IMPLEMENTATION
    #define NK_INCLUDE_FIXED_TYPES
    #define NK_INCLUDE_STANDARD_IO
    #define NK_INCLUDE_DEFAULT_ALLOCATOR
    #define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
    #define NK_INCLUDE_FONT_BAKING
    #define NK_INCLUDE_DEFAULT_FONT // TODO: Make custom font work.
    #define NK_INCLUDE_STANDARD_VARARGS
    #include <nuklear/nuklear.h>

    #undef SOKOL_NUKLEAR_IMPL
    #include <sokol/sokol_nuklear.h>
#endif

#endif 
