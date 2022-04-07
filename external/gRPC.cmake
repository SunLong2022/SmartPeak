include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set_property (DIRECTORY PROPERTY EP_BASE Dependencies)

set (DEPENDENCIES)
set (GRPC_VERSION v1.41.0)
message (STATUS "Adding gRPC ${GRPC_VERSION} as an external project.")

ExternalProject_Add(gRPC
  GIT_REPOSITORY https://github.com/grpc/grpc.git
  GIT_TAG        ${GRPC_VERSION}
  GIT_PROGRESS   TRUE
  CMAKE_ARGS 
    -Dabseil-cpp_BUILD_TESTS=OFF
    -DgRPC_BUILD_TESTS=OFF
    -DABSL_BUILD_TESTING=OFF
    -DABSL_RUN_TESTS=OFF
    -Dprotobuf_BUILD_TESTS=OFF
    -DgRPC_BUILD_GRPC_CSHARP_PLUGIN=OFF
    -DgRPC_BUILD_GRPC_NODE_PLUGIN=OFF
    -DgRPC_BUILD_GRPC_JAVA_PLUGIN=OFF
    -DgRPC_BUILD_GRPC_OBJECTIVE_C_PLUGIN=OFF
    -DgRPC_BUILD_GRPC_PHP_PLUGIN=OFF
    -DgRPC_BUILD_GRPC_PYTHON_PLUGIN=OFF
    -DgRPC_BUILD_GRPC_RUBY_PLUGIN=OFF
    -DABSL_USE_GOOGLETEST_HEAD=OFF
    -DABSL_USE_EXTERNAL_GOOGLETEST=OFF
    -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/install
  INSTALL_COMMAND cmake --install . --config Debug --prefix "${CMAKE_CURRENT_BINARY_DIR}/install"
)
