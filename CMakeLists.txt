cmake_minimum_required(VERSION 3.12.2)

# --------------------------------
# Make sure we can actually support the desired device  
# --------------------------------
set(XBEE_SUPPORTED_TARGETS "xbee_pro_s2")
if(NOT XBEE_TARGET)
    message(FATAL_ERROR "libxbee: Can't compile because XBEE_TARGET isn't specified!")
elseif("${XBEE_TARGET}" IN_LIST XBEE_SUPPORTED_TARGETS)
    message(STATUS "libxbee: Found supported device [${XBEE_TARGET}]")
else()
    message(FATAL_ERROR "libxbee: Device [${XBEE_TARGET}] was not found. Available devices are [${XBEE_SUPPORTED_TARGETS}]")
endif()


# --------------------------------
# Find the needed directories & files
# --------------------------------
set(XBEE_ROOT "${CMAKE_CURRENT_LIST_DIR}")

# Top level generic include/source
set(XBEE_INC_DIRS "${XBEE_ROOT}/libxbee")
set(XBEE_SRC_FILES
    "${XBEE_ROOT}/libxbee/xb_chimera_serial.cpp"
)

# Target specific include/source
if("${XBEE_TARGET}" STREQUAL "xbee_pro_s2")
    set(XBEE_INC_DIRS ${XBEE_INC_DIRS} "${XBEE_ROOT}/libxbee/modules/xbee_pro_s2")
    set(XBEE_SRC_FILES ${XBEE_SRC_FILES} "${XBEE_ROOT}/libxbee/modules/xbee_pro_s2/xbpros2.cpp")
endif()


# --------------------------------
# Build the target
# --------------------------------
project(libxbee)

