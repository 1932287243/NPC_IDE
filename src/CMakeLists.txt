find_package(Qt5 COMPONENTS Core Gui REQUIRED)

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
    ${PROJECT_SOURCE_DIR}/ui/mytextedit.ui
)

# 指定资源文件所在的目录
set(QRC_FILES
    ${PROJECT_SOURCE_DIR}/res/images.qrc
    ${PROJECT_SOURCE_DIR}/res/code.qrc
)

set(SOURCE_UI_DIR "${PROJECT_SOURCE_DIR}/src/ui")
file(GLOB SOURCE_UI_FILES "${SOURCE_UI_DIR}/*.cpp")
set(SOURCE_SDB_DIR "${PROJECT_SOURCE_DIR}/src/sdb")
file(GLOB SOURCE_SDB_FILES "${SOURCE_SDB_DIR}/*.cpp")
set(SOURCE_FILES ${SOURCE_UI_FILES} ${SOURCE_SDB_FILES} )

set(INCLUDE_UI_DIR "${PROJECT_SOURCE_DIR}/include/ui")
file(GLOB INCLUDE_UI_FILES "${INCLUDE_UI_DIR}/*.h")
set(INCLUDE_SDB_DIR "${PROJECT_SOURCE_DIR}/include/sdb")
file(GLOB INCLUDE_SDB_FILES "${INCLUDE_SDB_DIR}/*.h")
set(INCLUDE_FILES ${INCLUDE_UI_FILES} ${INCLUDE_SDB_FILES})

# add_executable(IDE 
#             ${SOURCES_FILES}
#             ${INCLUDE_FILES}
# )

# # add_dependencies(IDE CopyCommand)
# target_include_directories(IDE  PRIVATE ${PROJECT_SOURCE_DIR}/include/ui)
# # Qt5::Widgets 
# target_link_libraries(IDE PRIVATE Qt5::Core Qt5::Gui)

# if(Qt5PrintSupport_FOUND)
#     target_link_libraries(IDE PRIVATE Qt5::PrintSupport)
# endif()

# verilate(IDE
#   INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/vsrc
#   SOURCES CPU.v
#   VERILATOR_ARGS -O3 --x-assign fast --x-initial fast --noassert  --trace)


# set(IDE_TARGET IDE)
add_subdirectory(vsrc)
# target_link_libraries(IDE PRIVATE sm)

