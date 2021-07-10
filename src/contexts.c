/*
 * Copyright 2019, 2021 Denis Salem
 *
 * This file is part of DStudio.
 *
 * DStudio is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DStudio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DStudio. If not, see <http://www.gnu.org/licenses/>.
*/

#include "dstudio.h"

DStudioActiveContext    g_dstudio_active_contexts[3] = {0};
UIInteractiveList *     g_dstudio_contexts_lists[3]  = {0};
uint_fast32_t           g_dstudio_contexts_size[3]   = {0};
DStudioSetupClientContextFromListCallback * g_dstudio_setup_context_from_list[3] = {0};

void dstudio_update_current_context(uint_fast32_t index, uint_fast32_t context_level) { 
        DStudioGenericContext * generic_context = ((DStudioGenericContext *)g_dstudio_active_contexts[context_level].current);
        if (generic_context == NULL) {
            return;
        }
        
        DStudioContexts * parent = generic_context->parent;
        parent->index = index;
        g_dstudio_active_contexts[context_level].previous = g_dstudio_active_contexts[context_level].current;
        g_dstudio_active_contexts[context_level].current  = parent->data + (index* g_dstudio_contexts_size[context_level]);
        
        if (g_dstudio_setup_context_from_list[context_level]) {
            g_dstudio_setup_context_from_list[context_level]();
        }
}

uint_fast32_t dstudio_select_context_from_list(uint_fast32_t index, DStudioContextsLevel context_level) {
    DStudioGenericContext * generic_context = g_dstudio_active_contexts[context_level].current;
    if (generic_context == NULL) {
        return 0;
    }
    DStudioContexts * contexts = generic_context->parent;
    if(
        (index != contexts->index || 
            (
                g_dstudio_active_contexts[context_level].current != g_dstudio_active_contexts[context_level].previous
            )
        ) &&
        index < contexts->count
    ) {
        dstudio_update_current_context(index, context_level);
        return 1;
    }
    return 0;
}

void dstudio_setup_client_context(
    uint_fast32_t size,
    void (*sub_context_interactive_list_binder)(UIElements * lines, ListItemOpt flag),
    UIElements * (*sub_context_interactive_list_setter)(),
    DStudioSetupClientContextFromListCallback setup_client_context_callback_from_list
) {
    g_dstudio_contexts_size[DSTUDIO_INSTANCE_CONTEXTS_LEVEL] = sizeof(InstanceContext);
    g_dstudio_contexts_size[DSTUDIO_VOICE_CONTEXTS_LEVEL]   = sizeof(VoiceContext);
    g_dstudio_contexts_size[DSTUDIO_CLIENT_CONTEXTS_LEVEL]   = size;
    
    g_dstudio_setup_context_from_list[DSTUDIO_INSTANCE_CONTEXTS_LEVEL]  = dstudio_setup_instance_from_list;
    g_dstudio_setup_context_from_list[DSTUDIO_VOICE_CONTEXTS_LEVEL]     = dstudio_setup_voice_from_list;
    g_dstudio_setup_context_from_list[DSTUDIO_CLIENT_CONTEXTS_LEVEL]    = setup_client_context_callback_from_list;
    
    bind_sub_context_interactive_list = sub_context_interactive_list_binder;
    setup_sub_context_interactive_list = sub_context_interactive_list_setter;
}
