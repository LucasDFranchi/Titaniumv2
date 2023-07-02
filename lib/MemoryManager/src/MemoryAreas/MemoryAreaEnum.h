#ifndef MEMORY_AREA_ENUMERATIONS_GUARD
#define MEMORY_AREA_ENUMERATIONS_GUARD

/**
 * Enumeration of access types for a resource.
 */
enum access_type_e{
    READ_ONLY = 0,
    WRITE_ONLY = 1,
    READ_WRITE = 2,
};

/**
 * Enumeration of area indices.
 * 
 * @enum area_index_e
 * @constant DISPLAY_AREA The index of the display area.
 * @constant AREAS_COUNT The total number of areas.
 */
enum area_index_e{
    DISPLAY_AREA = 0,
    AREAS_COUNT = 1,
};

#endif /* MEMORY_AREA_ENUMERATIONS_GUARD */