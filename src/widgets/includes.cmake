set(SmartPeak_sources  CACHE INTERNAL "This variable should hold all SmartPeak sources at the end of the config step" )

## ATTENTION: The order of includes should be similar to the inclusion hierarchy
include(source/ui/sources.cmake)

set(SmartPeak_sources_h  CACHE INTERNAL "This variable should hold all SmartPeak sources at the end of the config step" )

## ATTENTION: The order of includes should be similar to the inclusion hierarchy
include(include/SmartPeak/ui/sources.cmake)

## add configured config.h&Co to source group
source_group("Header Files\\SmartPeak" FILES ${SmartPeak_configured_headers})
## merge all headers to sources (for source group view in VS)
list(APPEND SmartPeak_sources ${SmartPeak_sources_h} ${SmartPeak_configured_headers})

# TODO track why the duplicate warnings are thrown for all (!) MOC sources
# Macro problem?
list(REMOVE_DUPLICATES SmartPeak_sources)
