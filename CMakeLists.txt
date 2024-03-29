cmake_minimum_required(VERSION 3.5)

project(MessengerStatsNG
    VERSION 0.1.0
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
    LinguistTools
    Concurrent
    Multimedia
    MultimediaWidgets
    Charts
)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
    Widgets
    LinguistTools
    Concurrent
    Multimedia
    MultimediaWidgets
    Charts
)

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
        include/view/preferencesdialog.h
        src/view/preferencesdialog.cpp
        src/view/preferencesdialog.ui
        include/view/settings.h
        include/view/themeservice.h
        src/view/themeservice.cpp
        include/view/thread/threadinformationdialog.h
        src/view/thread/threadinformationdialog.cpp
        src/view/thread/threadinformationdialog.ui
        include/view/thread/nicknameitem.h
        src/view/thread/nicknameitem.cpp
        src/view/thread/nicknameitem.ui
        include/view/thread/searchdialog.h
        src/view/thread/searchdialog.cpp
        src/view/thread/searchdialog.ui
        include/view/gallery/gallerywidget.h
        src/view/gallery/gallerywidget.cpp
        src/view/gallery/gallerywidget.ui
        include/view/gallery/imagegalleryitem.h
        src/view/gallery/imagegalleryitem.cpp
        src/view/gallery/imagegalleryitem.ui
        include/view/gallery/videogalleryitem.h
        src/view/gallery/videogalleryitem.cpp
        src/view/gallery/videogalleryitem.ui
        include/view/gallery/audiogalleryitem.h
        src/view/gallery/audiogalleryitem.cpp
        src/view/gallery/audiogalleryitem.ui
        include/view/message/messageinformationdialog.h
        src/view/message/messageinformationdialog.cpp
        src/view/message/messageinformationdialog.ui
        include/view/personinformationdialog.h
        src/view/personinformationdialog.cpp
        src/view/personinformationdialog.ui
        include/view/statistics/statisticsviewer.h
        src/view/statistics/statisticsviewer.cpp
        src/view/statistics/statisticsviewer.ui
        include/view/statistics/statisticprovider.h
        src/view/statistics/statisticprovider.cpp
        include/view/statistics/impl/basic.h
        include/view/statistics/impl/barchart.h
        src/view/statistics/impl/barchart.cpp
        include/view/statistics/impl/chart.h
        src/view/statistics/impl/chart.cpp
        include/view/statistics/impl/messages.h
        src/view/statistics/impl/messages.cpp
        include/view/statistics/impl/piechart.h
        src/view/statistics/impl/piechart.cpp
        resources.qrc
)

set(VENDOR_SOURCES
    vendor/SlidingStackedWidget/slidingstackedwidget.cpp
    vendor/SlidingStackedWidget/slidingstackedwidget.h

    vendor/qtpropertybrowser/qtbuttonpropertybrowser.cpp
    vendor/qtpropertybrowser/qtbuttonpropertybrowser.h
    vendor/qtpropertybrowser/qteditorfactory.cpp
    vendor/qtpropertybrowser/qteditorfactory.h
    vendor/qtpropertybrowser/qtgroupboxpropertybrowser.cpp
    vendor/qtpropertybrowser/qtgroupboxpropertybrowser.h
    vendor/qtpropertybrowser/qtpropertybrowser.cpp
    vendor/qtpropertybrowser/qtpropertybrowser.h
    vendor/qtpropertybrowser/qtpropertybrowserutils.cpp
    vendor/qtpropertybrowser/qtpropertybrowserutils_p.h
    vendor/qtpropertybrowser/qtpropertymanager.cpp
    vendor/qtpropertybrowser/qtpropertymanager.h
    vendor/qtpropertybrowser/qttreepropertybrowser.cpp
    vendor/qtpropertybrowser/qttreepropertybrowser.h
    vendor/qtpropertybrowser/qtvariantproperty.cpp
    vendor/qtpropertybrowser/qtvariantproperty.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MessengerStatsNG
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${VENDOR_SOURCES}
        ${WIN32_RESOURCES}
    )

    qt6_add_translations(MessengerStatsNG
        TS_FILES ${TS_FILES}
        SOURCES ${PROJECT_SOURCES} ${VENDOR_SOURCES}
        LUPDATE_OPTIONS "-noobsolete"
    )
else()
    if(ANDROID)
        add_library(MessengerStatsNG SHARED ${PROJECT_SOURCES} ${VENDOR_SOURCES})
    else()
        add_executable(MessengerStatsNG ${PROJECT_SOURCES} ${VENDOR_SOURCES} ${WIN32_RESOURCES})
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()


add_dependencies(MessengerStatsNG MessengerStatsNG_lupdate)

target_link_libraries(MessengerStatsNG PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Concurrent
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::MultimediaWidgets
    Qt${QT_VERSION_MAJOR}::Charts
)

# Generate a header file so we can reference CMake version information in
# code
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/messengerstatsng/generated/version.h")

target_include_directories(MessengerStatsNG PRIVATE
    include
    vendor
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
