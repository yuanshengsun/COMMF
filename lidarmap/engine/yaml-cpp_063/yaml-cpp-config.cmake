get_filename_Component(YAML_CPP_CONFIG_PATH "${CMAKE_CURRENT_LIST_FILE}" PATH)

set(YAML_CPP_FOUND True)

set(YAML_CPP_INCLUDE_DIRS ${YAML_CPP_CONFIG_PATH}/include/)
if (MSVC) 
else()
  set(YAML_CPP_LIBS ${YAML_CPP_LIBRARIES}
      debug 
      ${YAML_CPP_CONFIG_PATH}/lib/Debug/libyaml-cppd.a
      optimized 
      ${YAML_CPP_CONFIG_PATH}/lib/Release/libyaml-cpp.a)
endif()
