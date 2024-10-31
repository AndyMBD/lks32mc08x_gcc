cmake_minimum_required(VERSION 3.16)
# 指定编译平台/架构与语言标准, 推荐指定Ninja为构建工具,可以加快编译速度(相比make)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
# 指定工具链
set(CMAKE_C_COMPILER_FORCED         TRUE) # skip compiler test
set(CMAKE_CXX_COMPILER_FORCED       TRUE)
set(CMAKE_C_COMPILER                arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER              arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER              arm-none-eabi-gcc)
set(CMAKE_OBJCOPY                   arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP                   arm-none-eabi-objdump)
set(SIZE                            arm-none-eabi-size) 
set(CMAKE_AR                        arm-none-eabi-ar)
set(CMAKE_CXX_STANDARD              17)
set(CMAKE_C_STANDARD                11)
set(CMAKE_EXPORT_COMPILE_COMMANDS   ON) 
set(CMAKE_EXECUTABLE_SUFFIX         "")
# 指定工程名称和语言类型
set(PROJECT_NAME                    AT089)
project(${PROJECT_NAME}             C CXX ASM) 

# 选择构建类型
# set(CMAKE_BUILD_TYPE Debug) # Debug Release RelWithDebInfo MinSizeRel

# board specific settings, arch/fpu/instruction
# set(MCU_FLAGS -mcpu=cortex-m0 -mthumb -mthumb-interwork -mfloat-abi=hard -mfpu=fpv4-sp-d16)
set(MCU_FLAGS -mcpu=cortex-m0 -mthumb -mthumb-interwork -mfloat-abi=soft)
# set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/LKS32MC03xxxx_FLASH.ld") # 指定链接脚本
# set(CMAKE_EXE_LINKER_FLAGS "-T LKS32MC03xxxx_FLASH.ld")
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/tools/gcc/LKS32MC081xxx_FLASH.ld")
# set(CMSISCORE "${CMAKE_SOURCE_DIR}/Drivers/CMSIS")
# set(DSP_NAME "libCMSISDSP.a") # 指定DSP库名称
# link_directories(${CMAKE_SOURCE_DIR}/ExtLibs/DSP/Lib)

# 构建选项
option(USE_EIGEN  OFF) # 是否使用Eigen库
option(BUILD_DSP OFF) # 是否编译DSP库,若要定制须修改DSP/source/CMakeLists.txt

if(BUILD_DSP)
    add_subdirectory(${CMAKE_SOURCE_DIR}/ExtLibs/DSP/source)
endif()

# Generic compiler settings for optimization and basic link lib
add_compile_options(-pipe ${MCU_FLAGS} -Og -Wall -Werror -fmessage-length=0 # basic options
                    -ffunction-sections -fdata-sections -fno-common # optimize options 
                    -Wno-comment # optimize options 
                    -Wno-implicit-function-declaration # disable implicit function declaration warning
                    -Wno-unused-parameter # disable unused parameter warning
                    )
add_link_options(-pipe ${MCU_FLAGS} -T${LINKER_SCRIPT} -Wl,--no-warn-rwx-segments # close RWX warning
                 -lc -lstdc++ -lm -lnosys # lib options
                 -Wl,--gc-sections -flto -specs=nano.specs -specs=nosys.specs # optimize options
                 -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map -Wl,--cref -Wl,--print-memory-usage # map options
                 ) # if your executable is too large , try option '-s' to strip symbols
# add_compile_definitions(__CC_ARM)
# add_compile_definitions() works for compile stage
# while add_definitions() works for both compile and link stage
# add_definitions(
#     -DUSE_HAL_DRIVER 
#     -DSTM32F407xx 
#     -DARM_MATH_CM4 
#     -DARM_MATH_MATRIX_CHECK 
#     -DARM_MATH_ROUNDING 
#     -DARM_MATH_LOOPUNROLL 
#     -DDISABLEFLOAT16
#     ) # need -D<macro> to define macro

# add inc
# include(${CMAKE_SOURCE_DIR}/ToolChain/find_header.cmake) 
# if(${USE_EIGEN})
#     include_sub_directories_recursively(${CMAKE_SOURCE_DIR}/ExtLibs/Eigen3)
# endif()

# include_sub_directories_recursively(${CMAKE_SOURCE_DIR}/include)
# include_sub_directories_recursively(${CMAKE_SOURCE_DIR}/lks32mc08x_periph_driver\Include)
# include_sub_directories_recursively(${CMAKE_SOURCE_DIR}/SEGGER_RTT)
# include_sub_directories_recursively(${CMAKE_SOURCE_DIR}/lks32mc0)
# Add include directories
include_directories(
    # include
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/LKS32MC08x
    ${CMAKE_SOURCE_DIR}/lks32mc08x_periph_driver/Include
    ${CMAKE_SOURCE_DIR}/lks32mc08x_periph_driver/source
    ${CMAKE_SOURCE_DIR}/RTE/_Target_1
    ${CMAKE_SOURCE_DIR}/SEGGER_RTT
)
# add source, only surfix .c .cpp
# file(GLOB_RECURSE SOURCE
file(GLOB SOURCE
          ${CMAKE_SOURCE_DIR}/AppFunction/*.c 
          ${CMAKE_SOURCE_DIR}/LKS32MC08x/*.c
          ${CMAKE_SOURCE_DIR}/lks32mc08x_periph_driver/source/*.c 
          ${CMAKE_SOURCE_DIR}/SEGGER_RTT/*.c
          ${CMAKE_SOURCE_DIR}/source/*.c
          ${CMAKE_SOURCE_DIR}/lks32mc08x_periph_driver/source/*.lib
)
message(STATUS "****************************************")
message(STATUS "SOURCE: ${CMAKE_SOURCE_DIR}")
message(STATUS "****************************************")
message(STATUS "SOURCE: ${SOURCE}")
message(STATUS "****************************************")
# file(GLOB_RECURSE SOURCE
#     "Drivers/*.c"
#     "Src/*.c"
#     "Middlewares/*.c"
#     "Core/*.cpp"
#     "Core/*.cc"
#     "Core/*.hpp"
# )

# 汇编文件路径
set(ASM_SOURCE
    # ${CMAKE_SOURCE_DIR}/LKS_SCOPE_v1.3.8/lksscope_demo_08x/startup_lks32mc08x.s
    # ${CMAKE_SOURCE_DIR}/startup_lks32mc03x_gcc.s
    ${CMAKE_SOURCE_DIR}/tools/gcc/startup_lks32mc08x_gcc.s
    # Middlewares/Third_Party/SEGGER/RTT/SEGGER_RTT_ASM_ARMv7M.s
    )
# set(ASM_SOURCE
#     Toolchain/startup_stm32f407xx.s
#     Middlewares/Third_Party/SEGGER/RTT/SEGGER_RTT_ASM_ARMv7M.s
#     )
set_source_files_properties(${ASM_SOURCE} PROPERTIES COMPILE_FLAGS "-x assembler-with-cpp")

# Build types
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(STATUS "Maximum optimization for speed")
    add_compile_options(-Ofast)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    message(STATUS "Maximum optimization for speed, debug info included")
    add_compile_options(-Ofast -g)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    message(STATUS "Maximum optimization for size")
    add_compile_options(-Os)
else ()
    message(STATUS "Minimal optimization, debug info included")
    add_compile_options(-Og -g -gdwarf-2)
    add_definitions(-DESC_DEBUG) # ESC Debug
endif ()

# build binary and hex file
# add_executable(${PROJECT_NAME}.elf  ${SOURCE} ${ASM_SOURCE} ${LINKER_SCRIPT})
add_executable(${PROJECT_NAME}.elf  
                        ${ASM_SOURCE} 
                        ${LINKER_SCRIPT}
                        ${SOURCE} )
# Specify linker script

# # Libraries to link
target_link_libraries(${PROJECT_NAME}.elf
                    ${CMAKE_SOURCE_DIR}/lks32mc08x_periph_driver/source/lks32mc08x_nvr.lib
)
# if(BUILD_DSP)
#     add_dependencies(${PROJECT_NAME}.elf CMSISDSP) # build DSP lib
# endif()

# target_link_libraries(${PROJECT_NAME}.elf ${DSP_NAME}) # link DSP lib

add_custom_command(
    TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin
    COMMENT "Building hex & bin file..."
    COMMENT "EXCUTABLE SIZE:"
    COMMAND ${SIZE} ${PROJECT_NAME}.elf
  )