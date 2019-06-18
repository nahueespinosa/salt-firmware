cmake_minimum_required(VERSION 3.14)
project(SALT)

set(CMAKE_CXX_STANDARD 14)

set(file_search_list
        "${CMAKE_HOME_DIRECTORY}/dimba/*.h"
        "${CMAKE_HOME_DIRECTORY}/dimba/*.c"
        "${CMAKE_HOME_DIRECTORY}/dimba/*.cpp"

        "${CMAKE_HOME_DIRECTORY}/salt/*.h"
        "${CMAKE_HOME_DIRECTORY}/salt/*.c"
        "${CMAKE_HOME_DIRECTORY}/salt/*.cpp"

        "${CMAKE_HOME_DIRECTORY}/third-party/ceedling/vendor/*.h"
        "${CMAKE_HOME_DIRECTORY}/third-party/ceedling/vendor/*.c"
        "${CMAKE_HOME_DIRECTORY}/third-party/ceedling/vendor/*.cpp"

        "${CMAKE_HOME_DIRECTORY}/third-party/jWrite/*.h"
        "${CMAKE_HOME_DIRECTORY}/third-party/jWrite/*.c"
        "${CMAKE_HOME_DIRECTORY}/third-party/jWrite/*.cpp"

        "${CMAKE_HOME_DIRECTORY}/third-party/LPCOpen/*.h"
        "${CMAKE_HOME_DIRECTORY}/third-party/LPCOpen/*.c"
        "${CMAKE_HOME_DIRECTORY}/third-party/LPCOpen/*.cpp"

        "${CMAKE_HOME_DIRECTORY}/third-party/MQTT-C/*.h"
        "${CMAKE_HOME_DIRECTORY}/third-party/MQTT-C/*.c"
        "${CMAKE_HOME_DIRECTORY}/third-party/MQTT-C/*.cpp"

        "${CMAKE_HOME_DIRECTORY}/third-party/rkh/*.h"
        "${CMAKE_HOME_DIRECTORY}/third-party/rkh/*.c"
        "${CMAKE_HOME_DIRECTORY}/third-party/rkh/*.cpp"

        "${CMAKE_HOME_DIRECTORY}/third-party/sapi/*.h"
        "${CMAKE_HOME_DIRECTORY}/third-party/sapi/*.c"
        "${CMAKE_HOME_DIRECTORY}/third-party/sapi/*.cpp"

        "${CMAKE_HOME_DIRECTORY}/third-party/ssp/*.h"
        "${CMAKE_HOME_DIRECTORY}/third-party/ssp/*.c"
        "${CMAKE_HOME_DIRECTORY}/third-party/ssp/*.cpp"
        )

set(file_remove_list
        "${CMAKE_HOME_DIRECTORY}/dimba/arch/win/*.h"
        "${CMAKE_HOME_DIRECTORY}/dimba/arch/win/*.c"
        "${CMAKE_HOME_DIRECTORY}/dimba/arch/win/*.cpp"

        "${CMAKE_HOME_DIRECTORY}/third-party/MQTT-C/examples/*.h"
        "${CMAKE_HOME_DIRECTORY}/third-party/MQTT-C/examples/*.c"
        "${CMAKE_HOME_DIRECTORY}/third-party/MQTT-C/examples/*.cpp"

        "${CMAKE_HOME_DIRECTORY}/third-party/rkh/demo/*.h"
        "${CMAKE_HOME_DIRECTORY}/third-party/rkh/demo/*.c"
        "${CMAKE_HOME_DIRECTORY}/third-party/rkh/demo/*.cpp"

        "${CMAKE_HOME_DIRECTORY}/third-party/rkh/doc/*.h"
        "${CMAKE_HOME_DIRECTORY}/third-party/rkh/doc/*.c"
        "${CMAKE_HOME_DIRECTORY}/third-party/rkh/doc/*.cpp"

        "${CMAKE_HOME_DIRECTORY}/third-party/rkh/template/*.h"
        "${CMAKE_HOME_DIRECTORY}/third-party/rkh/template/*.c"
        "${CMAKE_HOME_DIRECTORY}/third-party/rkh/template/*.cpp"

        "${CMAKE_HOME_DIRECTORY}/third-party/ssp/tests/*.h"
        "${CMAKE_HOME_DIRECTORY}/third-party/ssp/tests/*.c"
        "${CMAKE_HOME_DIRECTORY}/third-party/ssp/tests/*.cpp"
        )

MACRO(SOURCE_FILES_DIRECTORIES return_list)
    FILE(GLOB_RECURSE file_list ${file_search_list})
    FILE(GLOB_RECURSE file_remove_list ${file_remove_list})
    IF( file_remove_list )
        list(REMOVE_ITEM file_list ${file_remove_list})
    ENDIF()

    SET(dir_list "")
    FOREACH(file_path ${file_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        SET(dir_list ${dir_list} ${dir_path})
    ENDFOREACH()
    LIST(REMOVE_DUPLICATES dir_list)
    SET(${return_list} ${dir_list})
ENDMACRO()

MACRO(SOURCE_FILES return_list)

    FILE(GLOB_RECURSE file_list ${file_search_list})
    FILE(GLOB_RECURSE file_remove_list ${file_remove_list})
    IF( file_remove_list )
        list(REMOVE_ITEM file_list ${file_remove_list})
    ENDIF()

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

#FOREACH(file ${SRC_LIST})
#    message(${file})
#ENDFOREACH()

RKH_TEMPLETE_DEFINES()
#CESE_TEMPLETE_DEFINES()
remove_definitions(__unix__)

add_executable(clion ${SRC_LIST})

add_custom_target(generate_elf COMMAND make -C ./.. all)
