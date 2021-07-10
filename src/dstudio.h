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

#ifndef DSTUDIO_DOT_H_INCLUDED
#define DSTUDIO_DOT_H_INCLUDED

#include <FLAC/stream_decoder.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <jack/jack.h>
#include <jack/midiport.h>
#include <math.h>
#include <png.h>
#include <poll.h>
#include <pthread.h>
#include <pwd.h>
#include <semaphore.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <X11/X.h>
#include <X11/XKBlib.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>

#include "constants.h"
#include "macros.h"

typedef struct ControllerValue_t ControllerValue;
typedef enum   DStudioContextsLevel_t DStudioContextsLevel;
typedef struct DStudioWindowScale_t DStudioWindowScale;
typedef struct SharedSample_t SharedSample;
typedef struct UIElements_t UIElements;

#include "audio_api.h"
#include "bar_plot.h"
#include "buttons.h"
#include "common.h"
#include "contexts.h"
#include "extensions.h"
#include "fileutils.h"
#include "flac.h"
#include "instances.h"
#include "interactive_list.h"
#include "info_bar.h"
#include "knob.h"
#include "open_file.h"
#include "paths.h"
#include "samples.h"
#include "sliders.h"
#include "text.h"
#include "text_pointer.h"
#include "transition_animation.h"
#include "ui.h"
#include "voices.h"
#include "window_management.h"

#endif
