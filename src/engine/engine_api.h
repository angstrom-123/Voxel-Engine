#ifndef ENGINE_API
#define ENGINE_API 

#include "engine.h"
#include "event_system.h"
#include "raycast.h"

typedef struct engine_api {
    engine_t *engine_ptr;
} engine_api_t;

extern bool api_raycast_dda(engine_api_t *api, chunk_edit_e action, size_t range);
extern void api_subscribe_to_event(engine_api_t *api, event_type_e event, 
                                   const event_subscriber_desc_t *desc);

#endif

