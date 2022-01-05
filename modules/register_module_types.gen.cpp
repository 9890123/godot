// register_module_types.gen.cpp
/* THIS FILE IS GENERATED DO NOT EDIT */
#include "register_module_types.h"

#include "modules/modules_enabled.gen.h"

#include "modules/freetype/register_types.h"


void preregister_module_types() {
#ifdef MODULE_FREETYPE_ENABLED
#ifdef MODULE_FREETYPE_HAS_PREREGISTER
	preregister_freetype_types();
#endif
#endif

}

void register_module_types() {
#ifdef MODULE_FREETYPE_ENABLED
	register_freetype_types();
#endif

}

void unregister_module_types() {
#ifdef MODULE_FREETYPE_ENABLED
	unregister_freetype_types();
#endif

}
