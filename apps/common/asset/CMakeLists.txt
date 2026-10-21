include(${PROJECT_SOURCE_DIR}/cmake/third_party_yaml.cmake)

add_library(vulcal_app_asset STATIC
    asset_index.cpp
    asset_index.h
    image_loader.cpp
    image_loader.h
    material_file.cpp
    material_file.h
    material_importer.cpp
    material_importer.h
    obj_importer.cpp
    obj_importer.h
    obj_loader.cpp
    obj_loader.h
)

target_include_directories(vulcal_app_asset PUBLIC
    "${PROJECT_SOURCE_DIR}/apps"
)
target_link_libraries(vulcal_app_asset
    PUBLIC vulcal_engine
    PRIVATE yaml-cpp::yaml-cpp tinyobjloader::tinyobjloader stb::stb_image
)
