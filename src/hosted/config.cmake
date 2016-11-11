# EbbRT hosted platform-specific configuration
option(__EBBRT_HOSTED_DPDK_DRIVER__ "Enable DPDK Support" ON)
configure_file(${PLATFORM_SOURCE_DIR}/config.h.in config.h @ONLY)

# Build Settings
set(CMAKE_CXX_FLAGS "-Wall -Werror -std=gnu++14 -include ${CMAKE_CURRENT_BINARY_DIR}/config.h")

find_package(Boost 1.53.0 REQUIRED COMPONENTS 
  filesystem system coroutine context )
find_package(Capnp REQUIRED)
find_package(TBB REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CAPNP_INCLUDE_DIRS})
include_directories(${TBB_INCLUDE_DIRS})

set(EBBRT_DPDK_HEADERS 
    ${PROJECT_SOURCE_DIR}/native/Net.h
    ${PROJECT_SOURCE_DIR}/native/NetChecksum.h
    ${PROJECT_SOURCE_DIR}/native/NetDhcp.h
    ${PROJECT_SOURCE_DIR}/native/NetEth.h
    ${PROJECT_SOURCE_DIR}/native/NetIcmp.h
    ${PROJECT_SOURCE_DIR}/native/NetIp.h
    ${PROJECT_SOURCE_DIR}/native/NetIpAddress.h
    ${PROJECT_SOURCE_DIR}/native/NetMisc.h
    ${PROJECT_SOURCE_DIR}/native/NetTcp.h
    ${PROJECT_SOURCE_DIR}/native/NetUdp.h
    ${PROJECT_SOURCE_DIR}/native/Random.h
    ${PROJECT_SOURCE_DIR}/native/Rcu.h
    ${PROJECT_SOURCE_DIR}/native/RcuList.h
    ${PROJECT_SOURCE_DIR}/native/RcuTable.h
    ${PROJECT_SOURCE_DIR}/native/SharedPoolAllocator.h
    )
set(EBBRT_DPDK_SOURCES 
    ${PROJECT_SOURCE_DIR}/native/Net.cc
    ${PROJECT_SOURCE_DIR}/native/NetChecksum.cc
    ${PROJECT_SOURCE_DIR}/native/NetDhcp.cc
    ${PROJECT_SOURCE_DIR}/native/NetEth.cc
    ${PROJECT_SOURCE_DIR}/native/NetIcmp.cc
    ${PROJECT_SOURCE_DIR}/native/NetIp.cc
    ${PROJECT_SOURCE_DIR}/native/NetTcp.cc
    ${PROJECT_SOURCE_DIR}/native/NetUdp.cc
    ${PROJECT_SOURCE_DIR}/native/Random.cc
    )
