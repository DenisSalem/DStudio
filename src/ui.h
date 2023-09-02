typedef struct UIElements_t {
    GLchar                          vertex_indexes[4];
    GLuint                          vertex_array_object;
    GLuint                          vertex_buffer_object;
    GLuint                          index_buffer_object;
    GLfloat *                        alpha; 
    GLfloat *                        motions_buffer;
    Vec4                            vertex_attributes[4];
    UIElementsCoordinatesSettings   coordinates_settings;
    Vec4                            areas;
    UIElements *                    overlap_sub_menu_ui_elements;
    UIElementType                   type;
    UIInteractiveList *             interactive_list;
    void *                          application_callback_args;
    void                          (*application_callback)(UIElements * self);
    TransitionAnimation *           transition_animation;
} UIElements;
