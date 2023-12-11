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
 * @constant DISPLAY_AREA The index of the Display area.
 * @constant SSID_AREA The index of the SSID WiFi area.
 * @constant PASSWORD_AREA The index of the Password WiFi area.
 * @constant CONNECTION_AREA The index of the Connection Status WiFi area.
 * @constant AREAS_COUNT The total number of areas.
 */
enum area_index_e{
    DISPLAY_AREA    = 0,
    SSID_AREA       = 1,
    PASSWORD_AREA   = 2,
    CONNECTION_AREA = 3,
    LORA_WRITE_AREA = 4,
    LORA_READ_AREA  = 5,
    AREAS_COUNT,
};

#endif /* MEMORY_AREA_ENUMERATIONS_GUARD */