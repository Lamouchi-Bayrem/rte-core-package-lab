#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "RteCore::rte_core" for configuration "Debug"
set_property(TARGET RteCore::rte_core APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(RteCore::rte_core PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/librte_core.a"
  )

list(APPEND _cmake_import_check_targets RteCore::rte_core )
list(APPEND _cmake_import_check_files_for_RteCore::rte_core "${_IMPORT_PREFIX}/lib/librte_core.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
