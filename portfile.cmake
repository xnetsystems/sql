include(vcpkg_common_functions)

vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

set(SQLITE_VERSION 3300100)
set(SQLITE_HASH 030b53fe684e0fb8e9747b1f160e5e875807eabb0763caff66fe949ee6aa92f26f409b9b25034d8d1f5cee554a99e56a2bb92129287b0fe0671409babe9d18ea)

vcpkg_download_distfile(ARCHIVE
  URLS "https://sqlite.org/2019/sqlite-amalgamation-${SQLITE_VERSION}.zip"
  FILENAME "sqlite-amalgamation-${SQLITE_VERSION}.zip"
  SHA512 ${SQLITE_HASH}
)

vcpkg_extract_source_archive_ex(OUT_SOURCE_PATH SOURCE_PATH ARCHIVE ${ARCHIVE} REF ${SQLITE_VERSION})

file(COPY
  ${CURRENT_PORT_DIR}/include
  ${CURRENT_PORT_DIR}/src
  ${CURRENT_PORT_DIR}/CMakeLists.txt
  DESTINATION ${SOURCE_PATH})

vcpkg_configure_cmake(SOURCE_PATH ${SOURCE_PATH} PREFER_NINJA)

vcpkg_install_cmake()
vcpkg_fixup_cmake_targets(CONFIG_PATH lib/cmake/${PORT})

file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/include ${CURRENT_PACKAGES_DIR}/debug/share)
file(INSTALL ${CURRENT_PORT_DIR}/license.txt DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

vcpkg_copy_pdbs()
