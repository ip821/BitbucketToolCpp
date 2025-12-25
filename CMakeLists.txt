cmake_minimum_required(VERSION 4.0)
project(wxWidgetsPlayground LANGUAGES CXX OBJCXX)

include(FetchContent)

set(wxBUILD_SHARED OFF)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

FetchContent_Declare(
        wxWidgets
        GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
        GIT_SHALLOW ON
)
FetchContent_MakeAvailable(wxWidgets)

add_executable(wxWidgetsPlayground MACOSX_BUNDLE
        MyApp.cpp
        MyApp.h
        MyDialog.cpp
        MyDialog.h
        StatusItem.cpp
        StatusItem.h
        swift/file.swift
        swift/file.h
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
    # --- Swift library ---
    set(SWIFT_SRC ${CMAKE_SOURCE_DIR}/swift/file.swift)
    set(SWIFT_OUT_DIR ${CMAKE_BINARY_DIR}/swift)
    set(SWIFT_LIB ${SWIFT_OUT_DIR}/libswift_svc.dylib)

    file(MAKE_DIRECTORY ${SWIFT_OUT_DIR})

    add_custom_command(
            OUTPUT ${SWIFT_LIB}
            COMMAND xcrun swiftc
            -emit-library
            -o ${SWIFT_LIB}
            ${SWIFT_SRC}
            DEPENDS ${SWIFT_SRC}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            VERBATIM
    )

    add_custom_target(swift_svc ALL DEPENDS ${SWIFT_LIB})
    add_dependencies(wxWidgetsPlayground swift_svc)
    target_include_directories(wxWidgetsPlayground PRIVATE ${CMAKE_SOURCE_DIR}/swift)
    target_link_libraries(wxWidgetsPlayground PRIVATE ${SWIFT_LIB})
    # Let the app find the dylib at runtime
    set_target_properties(wxWidgetsPlayground PROPERTIES
            BUILD_RPATH "@executable_path"
            INSTALL_RPATH "@executable_path"
    )

    # Copy dylib next to the app binary after build
    add_custom_command(TARGET wxWidgetsPlayground POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${SWIFT_LIB}
            $<TARGET_FILE_DIR:wxWidgetsPlayground>
    )

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
