# DSTUDIO MEMORY MANAGEMENT

DStudio has it's own memory manager. It's a simple wrapper build around stantard
function like malloc, realloc and free. It MUST be the first dstudio call.

Function calls are not thread safe.
