set(FETCHCONTENT_QUIET OFF CACHE BOOL "Show FetchContent download and population logs")

include(${CMAKE_CURRENT_LIST_DIR}/third_party_logging.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/third_party_sdl3.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/third_party_glm.cmake)

set(VULCAO_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(VULCAO_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(VULCAO_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(third_party/vulcao)
