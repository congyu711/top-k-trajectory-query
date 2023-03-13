cmake_minimum_required(VERSION 3.5.1)

project(grpc_test C CXX)

include(./common.cmake)

# DOQU Proto file
get_filename_component(DOQU_proto "./DO_and_QU.proto" ABSOLUTE)
get_filename_component(DOQU_proto_path "${DOQU_proto}" PATH)
# QUCS1 Proto file
get_filename_component(QUCS1_proto "./CS1.proto" ABSOLUTE)
# get_filename_component(QUCS1_proto_path "${QUCS1_proto}" PATH)
# DOCS2 Proto file
get_filename_component(DOCS2_proto "./DO_and_CS2.proto" ABSOLUTE)
# get_filename_component(DOCS2_proto_path "${DOCS2_proto}" PATH)
# DOCS1 Proto file
get_filename_component(DOCS1_proto "./DO_and_CS1.proto" ABSOLUTE)
# get_filename_component(DOCS1_proto_path "${DOCS1_proto}" PATH)
# CS12 Proto file
get_filename_component(CS1CS2_proto "./CS1_and_CS2.proto" ABSOLUTE)
# get_filename_component(CS1CS2_proto_path "${CS1CS2_proto}" PATH)



# Generated sources
set(CMAKE_BUILD_TYPE "Debug")
set(DOQU_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/DO_and_QU.pb.cc")
set(DOQU_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/DO_and_QU.pb.h")
set(DOQU_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/DO_and_QU.grpc.pb.cc")
set(DOQU_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/DO_and_QU.grpc.pb.h")

set(QUCS1_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/CS1.pb.cc")
set(QUCS1_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/CS1.pb.h")
set(QUCS1_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/CS1.grpc.pb.cc")
set(QUCS1_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/CS1.grpc.pb.h")

set(DOCS2_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/DO_and_CS2.pb.cc")
set(DOCS2_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/DO_and_CS2.pb.h")
set(DOCS2_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/DO_and_CS2.grpc.pb.cc")
set(DOCS2_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/DO_and_CS2.grpc.pb.h")

set(DOCS1_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/DO_and_CS1.pb.cc")
set(DOCS1_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/DO_and_CS1.pb.h")
set(DOCS1_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/DO_and_CS1.grpc.pb.cc")
set(DOCS1_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/DO_and_CS1.grpc.pb.h")

set(CS1CS2_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/CS1_and_CS2.pb.cc")
set(CS1CS2_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/CS1_and_CS2.pb.h")
set(CS1CS2_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/CS1_and_CS2.grpc.pb.cc")
set(CS1CS2_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/CS1_and_CS2.grpc.pb.h")


add_custom_command(
      OUTPUT  "${DOQU_proto_srcs}" "${DOQU_proto_hdrs}" "${DOQU_grpc_srcs}" "${DOQU_grpc_hdrs}"
              "${QUCS1_proto_srcs}" "${QUCS1_proto_hdrs}" "${QUCS1_grpc_srcs}" "${QUCS1_grpc_hdrs}"
              "${DOCS2_proto_srcs}" "${DOCS2_proto_hdrs}" "${DOCS2_grpc_srcs}" "${DOCS2_grpc_hdrs}"
              "${DOCS1_proto_srcs}" "${DOCS1_proto_hdrs}" "${DOCS1_grpc_srcs}" "${DOCS1_grpc_hdrs}"
              "${CS1CS2_proto_srcs}" "${CS1CS2_proto_hdrs}" "${CS1CS2_grpc_srcs}" "${CS1CS2_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${DOQU_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${DOQU_proto}" "${QUCS1_proto}" "${DOCS2_proto}"  "${DOCS1_proto}" "${CS1CS2_proto}"
      DEPENDS "${DOQU_proto}" "${QUCS1_proto}" "${DOCS2_proto}"  "${DOCS1_proto}" "${CS1CS2_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# DOQU_grpc_proto
add_library(DOQU_grpc_proto
  ${DOQU_grpc_srcs}
  ${DOQU_grpc_hdrs}
  ${DOQU_proto_srcs}
  ${DOQU_proto_hdrs})
target_link_libraries(DOQU_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

# DOCS1_grpc_proto
add_library(DOCS1_grpc_proto
  ${DOCS1_grpc_srcs}
  ${DOCS1_grpc_hdrs}
  ${DOCS1_proto_srcs}
  ${DOCS1_proto_hdrs})
target_link_libraries(DOCS1_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

# DOCS2_grpc_proto
add_library(DOCS2_grpc_proto
  ${DOCS2_grpc_srcs}
  ${DOCS2_grpc_hdrs}
  ${DOCS2_proto_srcs}
  ${DOCS2_proto_hdrs})
target_link_libraries(DOCS2_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

# QUCS1_grpc_proto
add_library(QUCS1_grpc_proto
  ${QUCS1_grpc_srcs}
  ${QUCS1_grpc_hdrs}
  ${QUCS1_proto_srcs}
  ${QUCS1_proto_hdrs})
target_link_libraries(QUCS1_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

# CS1CS2_grpc_proto
add_library(CS1CS2_grpc_proto
  ${CS1CS2_grpc_srcs}
  ${CS1CS2_grpc_hdrs}
  ${CS1CS2_proto_srcs}
  ${CS1CS2_proto_hdrs})
target_link_libraries(CS1CS2_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})
  
# Targets greeter_[async_](client|server)
foreach(_target
  DataOwner QueryUser CloudServer1 CloudServer2)
  add_executable(${_target} "${_target}.cc")
  target_link_libraries(${_target}
    DOQU_grpc_proto
    DOCS1_grpc_proto
    DOCS2_grpc_proto
    CS1CS2_grpc_proto
    QUCS1_grpc_proto
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF}
    cryptopp)
endforeach()
