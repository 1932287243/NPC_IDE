# find_package(Qt5 COMPONENTS Core Gui REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui OpenGL)
# 查找PrintSupport库
find_package(Qt5 COMPONENTS PrintSupport)

find_package(verilator HINTS $ENV{VERILATOR_ROOT} ${VERILATOR_ROOT})
if (NOT verilator_FOUND)
  message(FATAL_ERROR "Verilator was not found. Either install it, or set the VERILATOR_ROOT environment variable")
endif()


# 启用自动处理选项
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
# set_property(SOURCE ${QRC_FILES} ${UI_FILES} PROPERTY SKIP_AUTOGEN ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/ui ${CMAKE_SOURCE_DIR}/res)

# 指定 UI 文件所在的目录
set(UI_FILES
    ${PROJECT_SOURCE_DIR}/ui/mainwindow.ui
    ${PROJECT_SOURCE_DIR}/ui/textedit.ui
)

# 指定资源文件所在的目录
set(QRC_FILES
    ${PROJECT_SOURCE_DIR}/res/images.qrc
    ${PROJECT_SOURCE_DIR}/res/code.qrc
    ${PROJECT_SOURCE_DIR}/res/resources.qrc
)

set(SOURCE_DEVICE_DIR "${PROJECT_SOURCE_DIR}/src/device")
file(GLOB SOURCE_DEVICE_FILES "${SOURCE_DEVICE_DIR}/*.cpp")

set(SOURCE_MONITOR_DIR "${PROJECT_SOURCE_DIR}/src/monitor")
file(GLOB SOURCE_MONITOR_FILES "${SOURCE_MONITOR_DIR}/*.cpp")

set(SOURCE_SDB_DIR "${PROJECT_SOURCE_DIR}/src/monitor/sdb")
file(GLOB SOURCE_SDB_FILES "${SOURCE_SDB_DIR}/*.cpp")

set(SOURCE_TOOLS_DIR "${PROJECT_SOURCE_DIR}/src/monitor/tools")
file(GLOB SOURCE_TOOLS_FILES "${SOURCE_TOOLS_DIR}/*.cpp")

set(SOURCE_UI_DIR "${PROJECT_SOURCE_DIR}/src/ui")
file(GLOB SOURCE_UI_FILES "${SOURCE_UI_DIR}/*.cpp")

set(SOURCE_UTILS_DIR "${PROJECT_SOURCE_DIR}/src/utils")
file(GLOB SOURCE_UTILS_FILES "${SOURCE_UTILS_DIR}/*.cpp")

# set(SOURCE_QTNODES_DIR "${PROJECT_SOURCE_DIR}/thirdpkg/QtNodes/src")
# file(GLOB SOURCE_QTNODES_FILES "${SOURCE_QTNODES_DIR}/*.cpp")
set(SOURCE_FILES  ${PROJECT_SOURCE_DIR}/src/main.cpp 
                  ${SOURCE_UTILS_FILES} 
                  ${SOURCE_UI_FILES} 
                  ${SOURCE_SDB_FILES} 
                  ${SOURCE_TOOLS_FILES}
                  ${SOURCE_MONITOR_FILES}
                  ${SOURCE_DEVICE_FILES})

set(INCLUDE_UI_DIR "${PROJECT_SOURCE_DIR}/include/ui")
file(GLOB INCLUDE_UI_FILES "${INCLUDE_UI_DIR}/*.h")

set(INCLUDE_MONITOR_DIR "${PROJECT_SOURCE_DIR}/include/monitor")
file(GLOB INCLUDE_MONITOR_FILES "${INCLUDE_MONITOR_DIR}/*.h")

set(INCLUDE_SDB_DIR "${PROJECT_SOURCE_DIR}/include/monitor/sdb")
file(GLOB INCLUDE_SDB_FILES "${INCLUDE_SDB_DIR}/*.h")

set(INCLUDE_TOOLS_DIR "${PROJECT_SOURCE_DIR}/include/monitor/tools")
file(GLOB INCLUDE_TOOLS_FILES "${INCLUDE_TOOLS_DIR}/*.h")

set(INCLUDE_UTILS_DIR "${PROJECT_SOURCE_DIR}/include/utils")
file(GLOB INCLUDE_UTILS_FILES "${INCLUDE_UTILS_DIR}/*.h")

set(INCLUDE_DEVICE_DIR "${PROJECT_SOURCE_DIR}/include/device")
file(GLOB INCLUDE_DEVICE_FILES "${INCLUDE_DEVICE_DIR}/*.h")

# message(${INCLUDE_UTILS_FILES})

set(INCLUDE_FILES   ${PROJECT_SOURCE_DIR}/include/main.h 
                    ${INCLUDE_UTILS_FILES} 
                    ${INCLUDE_UI_FILES} 
                    ${INCLUDE_MONITOR_FILES} 
                    ${INCLUDE_TOOLS_FILES} 
                    ${INCLUDE_SDB_FILES} 
                    ${INCLUDE_DEVICE_FILES})

add_subdirectory(vsrc)

