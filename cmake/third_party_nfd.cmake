include(FetchContent)

FetchContent_Declare(
    nativefiledialog
    GIT_REPOSITORY https://github.com/btzy/nativefiledialog-extended.git
    GIT_TAG        v1.4.1
    GIT_PROGRESS   TRUE
    GIT_SHALLOW    TRUE
)

set(NFD_BUILD_TESTS OFF CACHE BOOL "Build NativeFileDialog tests")
set(NFD_INSTALL OFF CACHE BOOL "Generate install target for NativeFileDialog")

FetchContent_MakeAvailable(nativefiledialog)
