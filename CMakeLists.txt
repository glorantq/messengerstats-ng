cmake_minimum_required(VERSION 3.5)

project(MessengerStatsNG
    VERSION 0.0.1
    DESCRIPTION "Utility to calculate statistics on a Facebook data takeout"
    HOMEPAGE_URL "https://glorantv.hu/"
    LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# On Windows we can do fancy things like giving the final executable an icon
# and embedding version metadata
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/win/version.rc.in
      ${CMAKE_CURRENT_BINARY_DIR}/win/version.rc
      @ONLY)

    enable_language("RC")
    set (WIN32_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/win/icons.rc
        ${CMAKE_CURRENT_BINARY_DIR}/win/version.rc
        ${CMAKE_CURRENT_SOURCE_DIR}/win/MessengerStatsNG.exe.manifest)
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS
    Widgets
    LinguistTools)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
    Widgets
    LinguistTools)

set(TS_FILES MessengerStatsNG_en_GB.ts)

set_source_files_properties(${TS_FILES}
    PROPERTIES OUTPUT_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/translations")

set(PROJECT_SOURCES
        src/main.cpp
        src/view/mainwindow.cpp
        include/view/mainwindow.h
        src/view/mainwindow.ui
        src/view/conversationspage.cpp
        src/view/conversationspage.ui
        include/view/conversationspage.h
        include/view/conversationlistitem.h
        src/view/conversationlistitem.cpp
        src/view/conversationlistitem.ui
        include/model/messengerdata.h
        src/model/messengerdata.cpp
        include/model/thread.h
        src/model/thread.cpp
        include/model/message.h
        src/model/message.cpp
        include/model/common.h
        include/view/threadpage.h
        src/view/threadpage.cpp
        src/view/threadpage.ui
        include/view/thread/threadlistmodel.h
        src/view/thread/threadlistmodel.cpp
        include/view/thread/messageitemdelegate.h
        src/view/thread/messageitemdelegate.cpp
        include/view/thread/renderers/genericmessagerenderer.h
        src/view/thread/renderers/genericmessagerenderer.cpp
        include/view/thread/renderers/callmessagerenderer.h
        src/view/thread/renderers/callmessagerenderer.cpp
        include/view/thread/renderers/centeredmessagerenderer.h
        src/view/thread/renderers/centeredmessagerenderer.cpp
        include/view/thread/renderers/fallbackmessagerenderer.h
        src/view/thread/renderers/fallbackmessagerenderer.cpp
        include/view/thread/renderers/imessagerenderer.h
        src/model/common.cpp
        resources.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MessengerStatsNG
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${WIN32_RESOURCES}
    )

    qt6_add_translations(MessengerStatsNG
        TS_FILES ${TS_FILES}
        SOURCES ${PROJECT_SOURCES}
        LUPDATE_OPTIONS "-noobsolete"
    )
else()
    if(ANDROID)
        add_library(MessengerStatsNG SHARED ${PROJECT_SOURCES})
    else()
        add_executable(MessengerStatsNG ${PROJECT_SOURCES} ${WIN32_RESOURCES})
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()


add_dependencies(MessengerStatsNG MessengerStatsNG_lupdate)

target_link_libraries(MessengerStatsNG PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
)

# Generate a header file so we can reference CMake version information in
# code
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/messengerstatsng/generated/version.h")

target_include_directories(MessengerStatsNG PRIVATE
    include
    "${CMAKE_CURRENT_BINARY_DIR}/messengerstatsng/"
)

set_target_properties(MessengerStatsNG PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER messengerstatsng.glorantv.hu
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MessengerStatsNG)
endif()
