cmake_minimum_required(VERSION 4.0)
project(PRToolForBitbucket2 VERSION 1.32 LANGUAGES CXX)

include(FetchContent)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(WX_WIDGETS_VERSION v3.3.3)
set(wxBUILD_SHARED OFF)
set(wxUSE_WEBREQUEST OFF)
set(wxUSE_WXHTML_HELP OFF)
set(wxUSE_STC OFF CACHE BOOL "" FORCE)

if (WIN32)
    set(wxUSE_WINRT ON CACHE BOOL "Enable native Windows toast notifications" FORCE)
endif ()

add_subdirectory(bitbucket_api)

if (APPLE)
    FetchContent_Declare(
            wxWidgets
            GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
            GIT_TAG ${WX_WIDGETS_VERSION}
            PATCH_COMMAND git apply "${CMAKE_CURRENT_SOURCE_DIR}/macos/wxwidgets-3_3_2.patch"
    )
elseif (MINGW)
    FetchContent_Declare(
            wxWidgets
            GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
            GIT_TAG ${WX_WIDGETS_VERSION}
            PATCH_COMMAND git apply "${CMAKE_CURRENT_SOURCE_DIR}/windows/wxwidgets-3_3_3-mingw-winrt.patch"
    )
else ()
    FetchContent_Declare(
            wxWidgets
            GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
            GIT_TAG ${WX_WIDGETS_VERSION}
    )
endif ()
FetchContent_MakeAvailable(wxWidgets)

FetchContent_Declare(
        cpp-utils
        GIT_REPOSITORY https://github.com/ip821/cpp-utils.git
)
FetchContent_MakeAvailable(cpp-utils)

FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.12.0
)
FetchContent_MakeAvailable(json)

add_executable(PRToolForBitbucket2 WIN32 MACOSX_BUNDLE
        App.cpp
        App.h
        menu/MenuBuilder.cpp
        menu/MenuBuilder.h
        menu/PullRequestsMenuBuilder.cpp
        menu/PullRequestsMenuBuilder.h
        preferences/PreferencesWindow.cpp
        preferences/PreferencesWindow.h
        StatusItem.cpp
        StatusItem.h
        Stopwatch.cpp
        Stopwatch.h
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
        wx::xrc
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
            $<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>:-Wl,--strip-all>
            -static
            -static-libgcc
            -static-libstdc++
    )
endif ()

# Resources
set(xrc_files
        ${CMAKE_CURRENT_SOURCE_DIR}/preferences/PreferencesWindowBase.xrc
        ${CMAKE_CURRENT_SOURCE_DIR}/preferences/wizard/WizardPages.xrc
)
if (NOT APPLE)
    list(APPEND xrc_files ${CMAKE_CURRENT_SOURCE_DIR}/images/resources.xrc)
endif ()
set(resource_cpp ${CMAKE_CURRENT_BINARY_DIR}/resources.cpp)

if (CMAKE_CROSSCOMPILING)
    set(HOST_WXRC "" CACHE FILEPATH "Path to a wxrc executable that runs on the build host")
    set(wxrc_dependencies)

    if (NOT HOST_WXRC)
        find_program(host_wxrc_from_path NAMES wxrc-3.3 wxrc)
        if (host_wxrc_from_path)
            set(HOST_WXRC "${host_wxrc_from_path}" CACHE FILEPATH
                    "Path to a wxrc executable that runs on the build host" FORCE)
        endif ()
    endif ()

    if (HOST_WXRC)
        execute_process(
                COMMAND "${HOST_WXRC}" -h
                RESULT_VARIABLE host_wxrc_result
                OUTPUT_QUIET
                ERROR_QUIET
        )
        if (NOT host_wxrc_result EQUAL 0)
            message(FATAL_ERROR "HOST_WXRC does not run on ${CMAKE_HOST_SYSTEM_NAME}: ${HOST_WXRC}")
        endif ()
    else ()
        include(ExternalProject)

        find_program(host_c_compiler NAMES clang REQUIRED NO_CMAKE_FIND_ROOT_PATH)
        find_program(host_cxx_compiler NAMES clang++ REQUIRED NO_CMAKE_FIND_ROOT_PATH)

        set(host_wxrc_prefix "${CMAKE_CURRENT_BINARY_DIR}/host-wxrc")
        set(host_wxrc_binary_dir "${host_wxrc_prefix}/build")
        set(HOST_WXRC "${host_wxrc_binary_dir}/lib/wxrc-3.3")

        ExternalProject_Add(host_wxrc
                PREFIX "${host_wxrc_prefix}"
                SOURCE_DIR "${wxwidgets_SOURCE_DIR}"
                BINARY_DIR "${host_wxrc_binary_dir}"
                DOWNLOAD_COMMAND ""
                UPDATE_COMMAND ""
                PATCH_COMMAND ""
                CONFIGURE_COMMAND
                "${CMAKE_COMMAND}"
                -S <SOURCE_DIR>
                -B <BINARY_DIR>
                -G "${CMAKE_GENERATOR}"
                -DCMAKE_BUILD_TYPE=Release
                -DCMAKE_C_COMPILER=${host_c_compiler}
                -DCMAKE_CXX_COMPILER=${host_cxx_compiler}
                -DwxBUILD_SHARED=OFF
                -DwxUSE_WEBREQUEST=OFF
                -DwxUSE_WXHTML_HELP=OFF
                -DwxUSE_STC=OFF
                BUILD_COMMAND
                "${CMAKE_COMMAND}" --build <BINARY_DIR> --target wxrc --config Release
                INSTALL_COMMAND ""
                BUILD_BYPRODUCTS "${HOST_WXRC}"
                USES_TERMINAL_CONFIGURE TRUE
                USES_TERMINAL_BUILD TRUE
        )

        list(APPEND wxrc_dependencies host_wxrc)
    endif ()

    set(wxrc "${HOST_WXRC}")
    list(APPEND wxrc_dependencies "${HOST_WXRC}")
else ()
    set(wxrc $<TARGET_FILE:wxrc>)
    set(wxrc_dependencies wxrc)
endif ()

add_custom_command(
        OUTPUT ${resource_cpp}
        COMMAND "${wxrc}" -c -o ${resource_cpp} ${xrc_files}
        DEPENDS ${xrc_files} ${wxrc_dependencies}
        COMMENT "Compiling XRC resources"
)
target_sources(PRToolForBitbucket2 PRIVATE ${resource_cpp} ${xrc_files})
# END Resources

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
