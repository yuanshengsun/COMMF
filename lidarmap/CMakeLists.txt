cmake_minimum_required(VERSION 3.0.0)
PROJECT(make_map)


set(yaml-cpp_DIR ${PROJECT_SOURCE_DIR}/engine/yaml-cpp_063)
# For YAML
find_package(yaml-cpp REQUIRED)
if (YAML_CPP_FOUND)
    set (INCLUDE_DIRS ${INCLUDE_DIRS}
        ${YAML_CPP_INCLUDE_DIRS}
    )
    set (LINK_LIBS ${LINK_LIBS}   
        ${YAML_CPP_LIBS}
    )
    set (DEFINATIONS ${DEFINATIONS}
        USE_YAML
    )
else ()
    message (WARNING "YAML_CPP not found!")
endif ()

# # Current directory
aux_source_directory(./engine/main SRC)

aux_source_directory(./engine/crc SRC)
aux_source_directory(./engine/daemon SRC)
aux_source_directory(./engine/factory SRC)
aux_source_directory(./engine/init SRC)
aux_source_directory(./engine/log_udp SRC)
aux_source_directory(./engine/message_core SRC)
aux_source_directory(./engine/wait SRC)
aux_source_directory(./engine/base SRC)

aux_source_directory(./node/component SRC)
aux_source_directory(./node/message SRC)

add_executable(make_map ${SRC})



# Target Info
target_include_directories(make_map
SYSTEM PRIVATE 
    ${INCLUDE_DIRS}
)

target_link_libraries(make_map 
PRIVATE
    ${LINK_LIBS}
)

target_compile_definitions(make_map
PRIVATE
    ${DEFINATIONS}
)