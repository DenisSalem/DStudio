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

#ifndef DSTUDIO_CONTEXTS_H_INCLUDED
#define DSTUDIO_CONTEXTS_H_INCLUDED

typedef struct DStudioGenericContext_t DStudioGenericContext;
typedef enum ListItemOpt_t ListItemOpt;
typedef struct UIElements_t UIElements;
typedef struct UIInteractiveList_t UIInteractiveList;

typedef enum DStudioContextsLevel_t{
    DSTUDIO_NONE_CONTEXTS_LEVEL     = -1,
    DSTUDIO_INSTANCE_CONTEXTS_LEVEL = 0,
    DSTUDIO_VOICE_CONTEXTS_LEVEL    = 1,
    DSTUDIO_CLIENT_CONTEXTS_LEVEL   = 2
} DStudioContextsLevel;

typedef struct DStudioActiveContext_t {
    DStudioGenericContext * previous;
    DStudioGenericContext * current;
} DStudioActiveContext;

typedef struct DStudioContexts_t {
    void * data;
    uint_fast32_t count;
    uint_fast32_t index;
} DStudioContexts;

typedef struct DStudioGenericContext_t {
    DSTUDIO_MANDATORY_CLIENT_CONTEXT_FIRST_ATTRIBUTES
} DStudioGenericContext;

extern DStudioActiveContext g_dstudio_active_contexts[3];
extern UIInteractiveList *  g_dstudio_contexts_lists[3];
extern uint_fast32_t        g_dstudio_contexts_size[3];

typedef void (DStudioSetupClientContextFromListCallback)();

extern DStudioSetupClientContextFromListCallback * g_dstudio_setup_context_from_list[3];

uint_fast32_t dstudio_select_context_from_list(uint_fast32_t index, DStudioContextsLevel context_level);

void dstudio_setup_client_context(
    uint_fast32_t size,
    void (*sub_context_interactive_list_binder)(UIElements * lines, ListItemOpt flag),
    UIElements * (*sub_context_interactive_list_setter)(),
    DStudioSetupClientContextFromListCallback setup_client_context_callback_from_list
);

void dstudio_update_current_context(uint_fast32_t context_index, uint_fast32_t context_level);

#endif
