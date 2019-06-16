cmake_minimum_required(VERSION 3.14)
project(SALT-FIRMWARE)

set(CMAKE_CXX_STANDARD 14)

MACRO(SOURCE_FILES_DIRECTORIES return_list)
    FILE(GLOB_RECURSE h_list
            "${CMAKE_HOME_DIRECTORY}/dimba/*.h"
            "${CMAKE_HOME_DIRECTORY}/third-party/ceedling/*.h"
            "${CMAKE_HOME_DIRECTORY}/third-party/jWrite/*.h"
            "${CMAKE_HOME_DIRECTORY}/third-party/LPCOpen/*.h"
            "${CMAKE_HOME_DIRECTORY}/third-party/MQTT-C/*.h"
            "${CMAKE_HOME_DIRECTORY}/third-party/rkh/*.h"
            "${CMAKE_HOME_DIRECTORY}/third-party/sapi/*.h"
            "${CMAKE_HOME_DIRECTORY}/third-party/ssp/*.h"
            )
    SET(file_list ${h_list})

    FILE(GLOB_RECURSE c_list
            "${CMAKE_HOME_DIRECTORY}/dimba/*.c"
            "${CMAKE_HOME_DIRECTORY}/third-party/ceedling/*.c"
            "${CMAKE_HOME_DIRECTORY}/third-party/jWrite/*.c"
            "${CMAKE_HOME_DIRECTORY}/third-party/LPCOpen/*.c"
            "${CMAKE_HOME_DIRECTORY}/third-party/MQTT-C/*.c"
            "${CMAKE_HOME_DIRECTORY}/third-party/rkh/*.c"
            "${CMAKE_HOME_DIRECTORY}/third-party/sapi/*.c"
            "${CMAKE_HOME_DIRECTORY}/third-party/ssp/*.c"
            )
    SET(file_list ${file_list} ${c_list})

    FILE(GLOB_RECURSE cpplist
            "${CMAKE_HOME_DIRECTORY}/dimba/*.cpp"
            "${CMAKE_HOME_DIRECTORY}/third-party/ceedling/*.cpp"
            "${CMAKE_HOME_DIRECTORY}/third-party/jWrite/*.cpp"
            "${CMAKE_HOME_DIRECTORY}/third-party/LPCOpen/*.cpp"
            "${CMAKE_HOME_DIRECTORY}/third-party/MQTT-C/*.cpp"
            "${CMAKE_HOME_DIRECTORY}/third-party/rkh/*.cpp"
            "${CMAKE_HOME_DIRECTORY}/third-party/sapi/*.cpp"
            "${CMAKE_HOME_DIRECTORY}/third-party/ssp/*.cpp"
            )
    SET(file_list ${file_list} ${cpplist})

    SET(dir_list "")
    FOREACH(file_path ${file_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        SET(dir_list ${dir_list} ${dir_path})
    ENDFOREACH()
    LIST(REMOVE_DUPLICATES dir_list)
    SET(${return_list} ${dir_list})
ENDMACRO()

MACRO(SOURCE_FILES return_list)
    FILE(GLOB_RECURSE h_list
            "${CMAKE_HOME_DIRECTORY}/dimba/*.h"
            "${CMAKE_HOME_DIRECTORY}/third-party/ceedling/*.h"
            "${CMAKE_HOME_DIRECTORY}/third-party/jWrite/*.h"
            "${CMAKE_HOME_DIRECTORY}/third-party/LPCOpen/*.h"
            "${CMAKE_HOME_DIRECTORY}/third-party/MQTT-C/*.h"
            "${CMAKE_HOME_DIRECTORY}/third-party/rkh/*.h"
            "${CMAKE_HOME_DIRECTORY}/third-party/sapi/*.h"
            "${CMAKE_HOME_DIRECTORY}/third-party/ssp/*.h"
            )
    SET(file_list ${h_list})

    FILE(GLOB_RECURSE c_list
            "${CMAKE_HOME_DIRECTORY}/dimba/*.c"
            "${CMAKE_HOME_DIRECTORY}/third-party/ceedling/*.c"
            "${CMAKE_HOME_DIRECTORY}/third-party/jWrite/*.c"
            "${CMAKE_HOME_DIRECTORY}/third-party/LPCOpen/*.c"
            "${CMAKE_HOME_DIRECTORY}/third-party/MQTT-C/*.c"
            "${CMAKE_HOME_DIRECTORY}/third-party/rkh/*.c"
            "${CMAKE_HOME_DIRECTORY}/third-party/sapi/*.c"
            "${CMAKE_HOME_DIRECTORY}/third-party/ssp/*.c"
            )
    SET(file_list ${file_list} ${c_list})

    FILE(GLOB_RECURSE cpp_list
            "${CMAKE_HOME_DIRECTORY}/dimba/*.cpp"
            "${CMAKE_HOME_DIRECTORY}/third-party/ceedling/*.cpp"
            "${CMAKE_HOME_DIRECTORY}/third-party/jWrite/*.cpp"
            "${CMAKE_HOME_DIRECTORY}/third-party/LPCOpen/*.cpp"
            "${CMAKE_HOME_DIRECTORY}/third-party/MQTT-C/*.cpp"
            "${CMAKE_HOME_DIRECTORY}/third-party/rkh/*.cpp"
            "${CMAKE_HOME_DIRECTORY}/third-party/sapi/*.cpp"
            "${CMAKE_HOME_DIRECTORY}/third-party/ssp/*.cpp"
            )
    SET(file_list ${file_list} ${cpp_list})

    SET(${return_list} ${file_list})
ENDMACRO()

MACRO(CESE_TEMPLETE_DEFINES)
    #CESE TEMPLETE

    #BOARD.MK file
    add_compile_definitions(BOARD=edu_ciaa_nxp)

    #PROGRAM.MK file
    add_compile_definitions(PROGRAM_PATH=examples/c/sapi/gpio)
    add_compile_definitions(PROGRAM_NAME=blinky)

    #CONFIG.MK file
    add_compile_definitions(VERBOSE=n)
    add_compile_definitions(OPT=g)
    add_compile_definitions(USE_NANO=y)
    add_compile_definitions(SEMIHOST=n)
    add_compile_definitions(USE_FPU=y)
    add_compile_definitions(USE_LPCOPEN=y)
    add_compile_definitions(USE_SAPI=y)
    add_compile_definitions(USE_FREERTOS=n)
    add_compile_definitions(FREERTOS_HEAP_TYPE=5)
    add_compile_definitions(LOAD_INRAM=n)

    #IDE FLAGS
    add_compile_definitions(__USE_LPCOPEN)
    add_compile_definitions(CHIP_LPC43XX)
    add_compile_definitions(ARM_MATH_CM4)
    add_compile_definitions(CORE_M4)
    add_compile_definitions(__USE_NEWLIB)
    add_compile_definitions(__FPU_PRESENT=1U)
    add_compile_definitions(USE_SAPI)
ENDMACRO()

MACRO(RKH_TEMPLETE_DEFINES)
    #RKH FRAMWORK

    #IDE FLAGS
    add_compile_definitions(__CODE_RED)
    add_compile_definitions(CORE_M4)
    add_compile_definitions(__LPC43XX__)
    add_compile_definitions(__REDLIB__)
    add_compile_definitions(__MULTICORE_NONE)
    add_compile_definitions(__CIAA_SAPI__=1)
    add_compile_definitions(MODPWR_CTRL_ENABLE=1)
    add_compile_definitions(__CIAANXP__)
    add_compile_definitions(__CIAA_NXP__=1)
    add_compile_definitions(ARCH_LITTLE_ENDIAN)
    add_compile_definitions(DEBUG=1)
    add_compile_definitions(GPRS_QUICK_SEND=1)
    add_compile_definitions(__REDLIB__)
ENDMACRO()


SOURCE_FILES_DIRECTORIES(header_directories)
SOURCE_FILES(SRC_LIST)

include_directories(${header_directories})

RKH_TEMPLETE_DEFINES()
#CESE_TEMPLETE_DEFINES()

add_executable(clion ${SRC_LIST})

add_custom_target(generate_elf COMMAND make -C ./.. all)
