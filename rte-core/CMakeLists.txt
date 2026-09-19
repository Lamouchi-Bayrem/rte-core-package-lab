cmake_minimum_required(VERSION 3.20)
project(RteCore VERSION 1.2.0 LANGUAGES C)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(CTest)
option(RTE_ENABLE_TRACE "Enable trace API and messages" OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
add_library(rte_core STATIC src/counter.c)
add_library(RteCore::rte_core ALIAS rte_core)
set_target_properties(rte_core PROPERTIES EXPORT_NAME rte_core)
target_include_directories(rte_core
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_options(rte_core PRIVATE $<$<C_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic>)
target_compile_definitions(rte_core PUBLIC $<$<BOOL:${RTE_ENABLE_TRACE}>:RTE_ENABLE_TRACE>)
if(BUILD_TESTING)
  add_executable(rte_core_test tests/test_counter.c)
  target_link_libraries(rte_core_test PRIVATE RteCore::rte_core)
  add_test(NAME rte_core.counter COMMAND rte_core_test)
endif()
set(RTE_CMAKE_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/RteCore")
configure_package_config_file(cmake/RteCoreConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/RteCoreConfig.cmake"
  INSTALL_DESTINATION "${RTE_CMAKE_DIR}")
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/RteCoreConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)
configure_file(cmake/rte-core.pc.in "${CMAKE_CURRENT_BINARY_DIR}/rte-core.pc" @ONLY)
install(TARGETS rte_core EXPORT RteCoreTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT RteCoreTargets FILE RteCoreTargets.cmake NAMESPACE RteCore:: DESTINATION "${RTE_CMAKE_DIR}")
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/RteCoreConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/RteCoreConfigVersion.cmake"
  DESTINATION "${RTE_CMAKE_DIR}")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/rte-core.pc" DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
set(CPACK_PACKAGE_NAME "rte-core")
set(CPACK_PACKAGE_VENDOR "Embedded Learning Lab")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_GENERATOR "TGZ")
include(CPack)
