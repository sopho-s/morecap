cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)

project(driver VERSION 0.1.0 LANGUAGES C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT KERNEL_RELEASE)
  execute_process(
    COMMAND uname -r
    OUTPUT_VARIABLE KERNEL_RELEASE
    OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

if(DEFINED ENV{KERNELDIR})
  set(KERNELHEADERS_DIR "$ENV{KERNELDIR}" CACHE PATH "Kernel build dir")
endif()

find_path(KERNELHEADERS_DIR Makefile
  PATHS /lib/modules/${KERNEL_RELEASE}/build
        /usr/src/linux-headers-${KERNEL_RELEASE}
        /usr/src/kernels/${KERNEL_RELEASE}
  NO_DEFAULT_PATH)

if(NOT KERNELHEADERS_DIR)
  message(FATAL_ERROR
    "Kernel headers not found for ${KERNEL_RELEASE}. "
    "Install them, or pass -DKERNELHEADERS_DIR=/path or set KERNELDIR.")
endif()


set(MODULE_NAME morecap)
set(MODULE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(MODULE_SOURCES ${MODULE_SRC_DIR}/main.c ${MODULE_SRC_DIR}/rulereader.h ${MODULE_SRC_DIR}/rulereader.c)
set(MODULE_OBJECT_FILES main.o rulereader.o)
list(JOIN MODULE_OBJECT_FILES " " MODULE_OBJECTS_STR)
set(DRIVER_FILE ${CMAKE_CURRENT_BINARY_DIR}/${MODULE_NAME}.ko)

file(WRITE ${MODULE_SRC_DIR}/Kbuild "obj-m := ${MODULE_NAME}.o\n${MODULE_NAME}-y := ${MODULE_OBJECTS_STR}\n")

add_custom_command(
  OUTPUT ${DRIVER_FILE}
  COMMAND $(MAKE) -C ${KERNELHEADERS_DIR}
          M=${MODULE_SRC_DIR}
          MO=${CMAKE_CURRENT_BINARY_DIR}
          modules
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS ${MODULE_SOURCES} ${MODULE_SRC_DIR}/Kbuild
  VERBATIM)

add_custom_target(driver ALL DEPENDS ${DRIVER_FILE})

add_library(dummy_target EXCLUDE_FROM_ALL ${MODULE_SOURCES})
target_compile_definitions(dummy_target PRIVATE __KERNEL__ MODULE)
target_include_directories(dummy_target PRIVATE ${KERNELHEADERS_DIR}/include)