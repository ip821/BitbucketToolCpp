cmake_minimum_required(VERSION 4.0)
project(wxWidgetsPlayground LANGUAGES CXX OBJCXX)

include(FetchContent)

set(wxBUILD_SHARED OFF)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

FetchContent_Declare(
        wxWidgets
        GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
        GIT_TAG v3.3.1
        GIT_SHALLOW ON
        PATCH_COMMAND git apply "${CMAKE_CURRENT_SOURCE_DIR}/macos/wxwidgets.patch"
        UPDATE_DISCONNECTED 1
)

FetchContent_MakeAvailable(wxWidgets)

add_executable(wxWidgetsPlayground MACOSX_BUNDLE
        MyApp.cpp
        MyApp.h
        MyDialog.cpp
        MyDialog.h
        StatusItem.cpp
        StatusItem.h
)
target_link_libraries(wxWidgetsPlayground PRIVATE wxcore wxnet)

set(STATUS_IMAGE_32_2x
        images/status32@2x.png
)
set(STATUS_IMAGE_32
        images/status32.png
)
set(STATUS_IMAGE_16
        images/status16.png
)

if (APPLE)
    set(APP_ICON "${CMAKE_SOURCE_DIR}/macos/AppIcon.icns")
    set_source_files_properties(${APP_ICON}
            PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
    )
    set_source_files_properties(${STATUS_IMAGE_32_2x}
            PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
    )
    set_source_files_properties(${STATUS_IMAGE_32}
            PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
    )
    set_source_files_properties(${STATUS_IMAGE_16}
            PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
    )
    set_target_properties(wxWidgetsPlayground PROPERTIES
            MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/info.plist"
            MACOSX_BUNDLE_BUNDLE_NAME "wxWidgetsPlayground"
            MACOSX_BUNDLE_GUI_IDENTIFIER "ip.wxWidgetsPlayground"
            MACOSX_BUNDLE_ICON_FILE "AppIcon.icns"
    )
    target_sources(wxWidgetsPlayground PRIVATE
            ${APP_ICON}
            ${STATUS_IMAGE_32_2x}
            ${STATUS_IMAGE_32}
            ${STATUS_IMAGE_16}
            macos/macos_activation.mm
    )
    target_link_libraries(wxWidgetsPlayground PRIVATE "-framework Cocoa")
endif ()
