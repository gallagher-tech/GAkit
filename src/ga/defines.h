
#define GA_OPENFRAMEWORKS
// #define GA_CINDER

// This enables glm's old behavior of initializing with non garbage values
// see openFrameworks issue #6530: https://github.com/openframeworks/openFrameworks/issues/6530
#define GLM_FORCE_CTOR_INIT

// -----------------------
// MACROS FOR CONVENIENCE:
// ---- DO NOT ADJUST ----

#define CLASS_INHERITS( TYPE, BASE ) class TYPE, typename = std::enable_if_t<std::is_base_of<BASE, TYPE>::value>
