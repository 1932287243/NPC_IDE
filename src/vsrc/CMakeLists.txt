# cmake_policy(SET CMP0079 OLD)
add_executable(IDE 
            ${SOURCE_FILES}
            ${INCLUDE_FILES}
            ${QRC_FILES}
            ${UI_FILES}
)

# message(${SOURCES_FILES})
# message(${INCLUDE_FILES})
# 添加编译器标志（例如，来自 llvm-config）
execute_process(
    COMMAND llvm-config-13 --cxxflags
    OUTPUT_VARIABLE LLVM_CXX_FLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
# message(${LLVM_CXX_FLAGS})

target_compile_options(IDE PRIVATE ${LLVM_CXX_FLAGS} -fPIE)

execute_process(
    COMMAND llvm-config-13 --includedir
    OUTPUT_VARIABLE LLVM_INCLUDE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

target_include_directories(IDE  PRIVATE ${PROJECT_SOURCE_DIR}/include
                                        ${PROJECT_SOURCE_DIR}/include/ui 
                                        ${PROJECT_SOURCE_DIR}/include/monitor
                                        ${PROJECT_SOURCE_DIR}/include/utils
                                        ${PROJECT_SOURCE_DIR}/include/device
					                    ${LLVM_INCLUDE})
# Qt5::Widgets 
target_link_libraries(IDE PRIVATE Qt5::Core Qt5::Gui)

if(Qt5PrintSupport_FOUND)
    target_link_libraries(IDE PRIVATE Qt5::PrintSupport)
endif()

# 获取 llvm-config 的库
execute_process(
    COMMAND llvm-config-13 --libs
    OUTPUT_VARIABLE LLVM_LIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
# message(${LLVM_LIBS})
# 添加链接库
target_link_libraries(IDE PRIVATE
    readline
    ${LLVM_LIBS}   # 链接来自 llvm-config 的库
)

verilate(IDE
  INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/vsrc
  SOURCES CPU.v
  VERILATOR_ARGS -O3 --x-assign fast --x-initial fast --noassert  --trace)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# file(APPEND "${PROJECT_SOURCE_DIR}/build/Makefile" "Your custom command here\n")
