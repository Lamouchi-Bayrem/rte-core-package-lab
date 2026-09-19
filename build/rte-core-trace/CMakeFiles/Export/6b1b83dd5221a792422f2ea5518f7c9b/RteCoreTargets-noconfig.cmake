#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "RteCore::rte_core" for configuration ""
set_property(TARGET RteCore::rte_core APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(RteCore::rte_core PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "C"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/librte_core.a"
  )

list(APPEND _cmake_import_check_targets RteCore::rte_core )
list(APPEND _cmake_import_check_files_for_RteCore::rte_core "${_IMPORT_PREFIX}/lib/librte_core.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
