#include "engine_api.h"

bool api_raycast_dda(engine_api_t *api, chunk_edit_e action, size_t range)
{
    cam_update(&api->engine_ptr->_render_sys.cam);
    return raycast_dda(&api->engine_ptr->_chunk_sys, action, &(raycast_desc_t) {
        .direction = cam_get_fwd(&api->engine_ptr->_render_sys.cam),
        .origin = api->engine_ptr->_render_sys.cam.pos,
        .range = range
    });
}

void api_subscribe_to_event(engine_api_t *api, event_type_e event, 
                            const event_subscriber_desc_t *desc)
{
    event_sys_subscribe_to_event(&api->engine_ptr->_event_sys, event, desc);
}
