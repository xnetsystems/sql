cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(sql VERSION 3.30.1 LANGUAGES C CXX)

file(GLOB_RECURSE headers CONFIGURE_DEPENDS include/*.hpp)
file(GLOB_RECURSE sources CONFIGURE_DEPENDS sqlite3ext.h sqlite3.h sqlite3.c src/*.hpp src/*.cpp)

add_library(sql STATIC ${headers} ${sources})
target_compile_features(sql PRIVATE cxx_std_20)
target_include_directories(sql PRIVATE src ${CMAKE_CURRENT_SOURCE_DIR} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

if(WIN32)
  target_compile_definitions(sql PRIVATE _UNICODE UNICODE WIN32_LEAN_AND_MEAN NOMINMAX)
endif()

target_compile_definitions(sql PRIVATE
  SQLITE_DQS=0
  SQLITE_THREADSAFE=2
  SQLITE_DEFAULT_MEMSTATUS=0
  SQLITE_DEFAULT_SYNCHRONOUS=1
  SQLITE_LIKE_DOESNT_MATCH_BLOBS
  SQLITE_MAX_EXPR_DEPTH=0
  SQLITE_OMIT_DEPRECATED
  SQLITE_OMIT_PROGRESS_CALLBACK
  SQLITE_OMIT_SHARED_CACHE
  SQLITE_ENABLE_COLUMN_METADATA
  SQLITE_USE_ALLOCA)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_link_libraries(sql PUBLIC dl)
endif()

install(DIRECTORY include/ DESTINATION include FILES_MATCHING PATTERN "*.hpp")
install(TARGETS sql EXPORT sql LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(EXPORT sql FILE sql-config.cmake NAMESPACE sql:: DESTINATION lib/cmake/sql)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/sql-config-version.cmake
  VERSION ${PROJECT_VERSION} COMPATIBILITY SameMajorVersion)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/sql-config-version.cmake DESTINATION lib/cmake/sql)

add_library(sql::sql ALIAS sql)
