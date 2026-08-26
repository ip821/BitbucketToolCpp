cmake_minimum_required(VERSION 4.0)
project(PRToolForBitbucket2 VERSION 1.35 LANGUAGES CXX)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Dependencies)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(WX_WIDGETS_VERSION v3.3.3)
set(wxBUILD_SHARED OFF CACHE BOOL "" FORCE)
set(wxBUILD_MONOLITHIC OFF CACHE BOOL "" FORCE)
set(wxBUILD_INSTALL OFF CACHE BOOL "" FORCE)
set(wxBUILD_LOCALES OFF CACHE STRING "" FORCE)

foreach(wx_option IN ITEMS
        wxUSE_ACTIVEX
        wxUSE_ARCHIVE_STREAMS
        wxUSE_ARTPROVIDER_STD
        wxUSE_ARTPROVIDER_TANGO
        wxUSE_AUI
        wxUSE_DATAVIEWCTRL
        wxUSE_FS_ARCHIVE
        wxUSE_FS_INET
        wxUSE_FS_ZIP
        wxUSE_GIF
        wxUSE_GRID
        wxUSE_HTML
        wxUSE_IFF
        wxUSE_JOYSTICK
        wxUSE_MEDIACTRL
        wxUSE_OPENGL
        wxUSE_PCX
        wxUSE_PNM
        wxUSE_POSTSCRIPT
        wxUSE_PRINTING_ARCHITECTURE
        wxUSE_PROPGRID
        wxUSE_PROTOCOL
        wxUSE_PROTOCOL_FILE
        wxUSE_PROTOCOL_FTP
        wxUSE_PROTOCOL_HTTP
        wxUSE_RIBBON
        wxUSE_RICHTEXT
        wxUSE_SOCKETS
        wxUSE_SOUND
        wxUSE_STC
        wxUSE_SVG
        wxUSE_TARSTREAM
        wxUSE_TGA
        wxUSE_TREELISTCTRL
        wxUSE_URL
        wxUSE_WEBREQUEST
        wxUSE_WEBVIEW
        wxUSE_WXHTML_HELP
        wxUSE_XML
        wxUSE_XPM
        wxUSE_XRC
        wxUSE_ZIPSTREAM
)
    set(${wx_option} OFF CACHE BOOL "" FORCE)
endforeach()

foreach(wx_library IN ITEMS
        wxUSE_EXPAT
        wxUSE_LIBJPEG
        wxUSE_LIBTIFF
        wxUSE_LIBWEBP
        wxUSE_LUNASVG
        wxUSE_NANOSVG
        wxUSE_REGEX
)
    set(${wx_library} OFF CACHE STRING "" FORCE)
endforeach()

# PNG decoding is only needed for the embedded GTK status icon. Windows loads
# its icon from the PE resources and macOS loads it from the application bundle.
if (WIN32 OR APPLE)
    set(wxUSE_LIBPNG OFF CACHE STRING "" FORCE)
    set(wxUSE_ZLIB OFF CACHE STRING "" FORCE)
else ()
    set(wxUSE_LIBPNG builtin CACHE STRING "" FORCE)
    set(wxUSE_ZLIB builtin CACHE STRING "" FORCE)
endif ()

# The Bitbucket client only performs HTTP(S) requests. Keep libcurl static and
# avoid detecting optional Homebrew/system libraries that would add runtime
# dependencies to the macOS bundle.
set(HTTP_ONLY ON CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CURL_DISABLE_INSTALL ON CACHE BOOL "" FORCE)
set(CURL_USE_LIBSSH OFF CACHE BOOL "" FORCE)
set(CURL_USE_LIBSSH2 OFF CACHE BOOL "" FORCE)
set(CURL_USE_LIBPSL OFF CACHE BOOL "" FORCE)
set(CURL_USE_PKGCONFIG OFF CACHE BOOL "" FORCE)
set(CURL_BROTLI OFF CACHE STRING "" FORCE)
set(CURL_ZLIB OFF CACHE STRING "" FORCE)
set(CURL_ZSTD OFF CACHE STRING "" FORCE)
set(USE_LIBIDN2 OFF CACHE BOOL "" FORCE)
set(USE_NGHTTP2 OFF CACHE BOOL "" FORCE)

if (UNIX)
    set(OPENSSL_USE_STATIC_LIBS ON CACHE BOOL "" FORCE)
endif ()

# Put each function and datum in its own section so the final executable can
# discard unused code from all statically linked dependencies.
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
            $<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>:-ffunction-sections>
            $<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>:-fdata-sections>
    )
endif ()

if (WIN32)
    set(wxUSE_WINRT ON CACHE BOOL "Enable native Windows toast notifications" FORCE)
endif ()

add_subdirectory(bitbucket_api)

add_dependency(
        wxWidgets

        GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
        GIT_TAG ${WX_WIDGETS_VERSION}
        GIT_SHALLOW TRUE

        PATCH_FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/macos/wxwidgets-3_3_2.patch"
            "${CMAKE_CURRENT_SOURCE_DIR}/windows/wxwidgets-3_3_3-mingw-winrt.patch"
)

add_dependency(
        cpp-utils

        GIT_REPOSITORY https://github.com/ip821/cpp-utils.git
)

add_dependency(
        json

        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.12.0
)

add_executable(PRToolForBitbucket2 WIN32 MACOSX_BUNDLE
        App.cpp
        App.h
        menu/MenuBuilder.cpp
        menu/MenuBuilder.h
        menu/PullRequestsMenuBuilder.cpp
        menu/PullRequestsMenuBuilder.h
        preferences/PreferencesWindowBase.cpp
        preferences/PreferencesWindowBase.h
        preferences/PreferencesWindow.cpp
        preferences/PreferencesWindow.h
        StatusItem.cpp
        StatusItem.h
        Stopwatch.cpp
        Stopwatch.h
        preferences/wizard/WizardPages.cpp
        preferences/wizard/WizardPages.h
        preferences/wizard/SetupWizard.cpp
        preferences/wizard/SetupWizard.h
        preferences/wizard/LoginPage.cpp
        preferences/wizard/LoginPage.h
        preferences/wizard/WorkspacePage.cpp
        preferences/wizard/WorkspacePage.cpp
        preferences/wizard/WorkspacePage.h
        preferences/wizard/RepositoryPage.cpp
        preferences/wizard/RepositoryPage.h
        preferences/wizard/SetupWizardContext.h
        preferences/settings/Config.cpp
        preferences/settings/Config.h
        preferences/Credentials.cpp
        preferences/Credentials.h
        preferences/wizard/LoginView.cpp
        preferences/wizard/LoginView.h
        preferences/wizard/WorkspaceView.cpp
        preferences/wizard/WorkspaceView.h
        preferences/wizard/RepositoryView.cpp
        preferences/wizard/RepositoryView.h
        pull_requests/PullRequestService.cpp
        pull_requests/PullRequestService.h
        pull_requests/PullRequestInfo.h
        version.h
        main.cpp
        pull_requests/PullRequestsInfo.h
        menu/MenuMetrics.h
        menu/PullRequestMenuEntry.h
        menu/PullRequestMenuEntry.cpp
)
configure_file(
        ${CMAKE_SOURCE_DIR}/version.h.in
        ${CMAKE_BINARY_DIR}/version.h
        @ONLY
)
target_include_directories(PRToolForBitbucket2 PRIVATE ${CMAKE_BINARY_DIR})
target_compile_options(PRToolForBitbucket2 PRIVATE
        -Wall -Wextra -Wpedantic -Werror
)
target_link_libraries(PRToolForBitbucket2 PRIVATE
        wx::core
        nlohmann_json::nlohmann_json
        cpp_utils::cpp_utils
        bitbucket_api::bitbucket_api
)

if (WIN32)
    target_sources(PRToolForBitbucket2 PRIVATE
            windows/app.rc
            windows/app.manifest
            windows/CustomIcon.cpp
            windows/CustomIcon.h
    )
    set_source_files_properties(windows/app.rc PROPERTIES LANGUAGE RC)
    set_property(SOURCE windows/app.rc APPEND PROPERTY OBJECT_DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/windows/app.ico"
    )
    target_link_options(PRToolForBitbucket2 PRIVATE
            -static
            -static-libgcc
            -static-libstdc++
    )
endif ()

# Embed the GTK status icon directly in the executable without XRC/XML.
if (NOT WIN32 AND NOT APPLE)
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/images/status32.png" status_image_hex HEX)
    string(REGEX REPLACE "(..)" "0x\\1," status_image_data "${status_image_hex}")
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/generated")
    configure_file(
            images/EmbeddedStatusImage.h.in
            "${CMAKE_CURRENT_BINARY_DIR}/generated/EmbeddedStatusImage.h"
            @ONLY
    )
    target_sources(PRToolForBitbucket2 PRIVATE
            images/StatusImage.cpp
            images/StatusImage.h
    )
endif ()

# Discard unused release sections using each platform linker's native spelling.
if (APPLE)
    target_link_options(PRToolForBitbucket2 PRIVATE
            $<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>:LINKER:-dead_strip>
    )
else ()
    target_link_options(PRToolForBitbucket2 PRIVATE
            $<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>:LINKER:--gc-sections>
    )
endif ()

if (CMAKE_STRIP)
    # Run the platform strip tool after linking Release and MinSizeRel builds.
    add_custom_command(TARGET PRToolForBitbucket2 POST_BUILD
            COMMAND "${CMAKE_COMMAND}"
                    "-DSTRIP_COMMAND=${CMAKE_STRIP}"
                    "-DEXECUTABLE=$<TARGET_FILE:PRToolForBitbucket2>"
                    "-DCONFIGURATION=$<CONFIG>"
                    -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/StripExecutable.cmake"
            COMMENT "Stripping release executable"
            VERBATIM
    )
endif ()

set(STATUS_IMAGE_32_2x
        macos/status32@2x.png
)
set(STATUS_IMAGE_32
        images/status32.png
)
set(STATUS_IMAGE_16
        images/status16.png
)

if (APPLE)
    project(PRToolForBitbucket2 LANGUAGES CXX OBJCXX)
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
    set_target_properties(PRToolForBitbucket2 PROPERTIES
            MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/macos/info.plist"
            MACOSX_BUNDLE_BUNDLE_NAME "PRToolForBitbucket2"
            MACOSX_BUNDLE_GUI_IDENTIFIER "ip.PRToolForBitbucket2"
            MACOSX_BUNDLE_ICON_FILE "AppIcon.icns"
    )
    target_sources(PRToolForBitbucket2 PRIVATE
            ${APP_ICON}
            ${STATUS_IMAGE_32_2x}
            ${STATUS_IMAGE_32}
            ${STATUS_IMAGE_16}
            macos/macos_activation.mm
            macos/menubar.mm
    )
    target_compile_options(PRToolForBitbucket2 PRIVATE
            -fno-builtin-std-forward_like
    )
    target_link_libraries(PRToolForBitbucket2 PRIVATE "-framework Cocoa")
endif ()
