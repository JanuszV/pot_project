idf_component_register(SRCS "main.cpp" "dht22.cpp" "soil_moisture.cpp"
                      INCLUDE_DIRS ".")
# Set C++11 standard
target_compile_features(${COMPONENT_LIB} PRIVATE cxx_std_11)
