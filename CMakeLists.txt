cmake_minimum_required(VERSION 4.0)
project(wxWidgetsPlayground LANGUAGES CXX)

include(FetchContent)

set(wxBUILD_SHARED OFF)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(WX_WIDGETS_VERSION v3.3.1)

if(APPLE)
    FetchContent_Declare(
            wxWidgets
            GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
            GIT_TAG ${WX_WIDGETS_VERSION}
            GIT_SHALLOW ON
            PATCH_COMMAND git apply "${CMAKE_CURRENT_SOURCE_DIR}/macos/wxwidgets.patch"
            UPDATE_DISCONNECTED 1
    )
else ()
    FetchContent_Declare(
            wxWidgets
            GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
            GIT_TAG ${WX_WIDGETS_VERSION}
            GIT_SHALLOW ON
    )
endif ()

FetchContent_MakeAvailable(wxWidgets)

add_executable(wxWidgetsPlayground MACOSX_BUNDLE
        MyApp.cpp
        MyApp.h
        preferences/PreferencesWindow.cpp
        preferences/PreferencesWindow.h
        StatusItem.cpp
        StatusItem.h
        windows/app.rc
        windows/app.manifest
        preferences/wizard/SetupWizard.cpp
        preferences/wizard/SetupWizard.h
        stdafx.h
        stdafx.h
        preferences/wizard/LoginPage.cpp
        preferences/wizard/LoginPage.h
        preferences/wizard/WorkspacePage.cpp
        preferences/wizard/WorkspacePage.cpp
        preferences/wizard/WorkspacePage.h
        preferences/wizard/RepositoryPage.cpp
        preferences/wizard/RepositoryPage.h
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
    project(wxWidgetsPlayground LANGUAGES CXX OBJCXX)
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

if (WIN32)
    target_sources(wxWidgetsPlayground PRIVATE
            windows/app.rc
    )
    set_source_files_properties(windows/app.rc PROPERTIES LANGUAGE RC)
endif ()