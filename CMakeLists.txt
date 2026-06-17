cmake_minimum_required(VERSION 4.0)
project(PRToolForBitbucket2 LANGUAGES CXX)

include(FetchContent)

set(BITBACKETTOOL_APP_VERSION 1.4)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(WX_WIDGETS_VERSION v3.3.2)
set(wxBUILD_SHARED OFF)
set(wxUSE_STC OFF CACHE BOOL "" FORCE)

if (APPLE)
    FetchContent_Declare(
            wxWidgets
            GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
            GIT_TAG ${WX_WIDGETS_VERSION}
            GIT_SHALLOW ON
            PATCH_COMMAND git apply "${CMAKE_CURRENT_SOURCE_DIR}/macos/wxwidgets-3_3_2.patch"
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

FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.12.0
)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
        cpp-utils
        GIT_REPOSITORY https://github.com/ip821/cpp-utils.git
)
FetchContent_MakeAvailable(cpp-utils)

add_executable(PRToolForBitbucket2 WIN32 MACOSX_BUNDLE
        App.cpp
        App.h
        preferences/PreferencesWindowBase.cpp
        preferences/PreferencesWindowBase.h
        preferences/PreferencesWindow.cpp
        preferences/PreferencesWindow.h
        StatusItem.cpp
        StatusItem.h
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
        http/HttpConnection.h
        http/HttpConnection.cpp
        webrequests/WorkspacesRequest.h
        webrequests/RepositoriesRequest.h
        webrequests/PullRequestsRequest.h
        webrequests/Repository.h
        webrequests/Workspace.h
        preferences/wizard/LoginView.cpp
        preferences/wizard/LoginView.h
        preferences/wizard/WorkspaceView.cpp
        preferences/wizard/WorkspaceView.h
        preferences/wizard/RepositoryView.cpp
        preferences/wizard/RepositoryView.h
        webrequests/PullRequest.h
        webrequests/User.h
        webrequests/ParticipantUser.h
        webrequests/Link.h
        webrequests/CurrentUserRequest.h
        webrequests/Values.h
        pull_requests/PullRequestService.cpp
        pull_requests/PullRequestService.h
        pull_requests/PullRequestInfo.h
        webrequests/BitbucketRequest.h
        webrequests/PullRequestRequest.h
        webrequests/DiffStat.h
        webrequests/DiffStatRequest.h
        webrequests/Status.h
        webrequests/StatusRequest.h
        version.h
        main.cpp
        webrequests/ServerResponse.h
        webrequests/BitbucketUrlBuilder.h
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
        wx::net
        nlohmann_json::nlohmann_json
        cpp_utils::cpp_utils)

if (WIN32)
    target_link_options(PRToolForBitbucket2 PRIVATE
            $<$<CONFIG:Release>:-Wl,--strip-all>
    )
    target_sources(PRToolForBitbucket2 PRIVATE
            windows/app.rc
            windows/app.manifest
    )
    set_source_files_properties(windows/app.rc PROPERTIES LANGUAGE RC)
    get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)

    add_custom_command(TARGET PRToolForBitbucket2 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${MINGW_BIN_DIR}/libgcc_s_seh-1.dll"
            "${MINGW_BIN_DIR}/libstdc++-6.dll"
            "${MINGW_BIN_DIR}/libwinpthread-1.dll"
            "$<TARGET_FILE_DIR:PRToolForBitbucket2>"
            COMMENT "Copying MinGW runtime DLLs"
    )
#    target_link_options(PRToolForBitbucket2 PRIVATE
#            -static
#            -static-libgcc
#            -static-libstdc++
#    )
endif ()

if (NOT APPLE)
    # Resources
    set(xrc_files ${CMAKE_CURRENT_SOURCE_DIR}/images/resources.xrc)
    set(resource_cpp ${CMAKE_CURRENT_BINARY_DIR}/resources.cpp)
    set(wxrc $<TARGET_FILE:wxrc>)

    add_custom_command(
            OUTPUT ${resource_cpp}
            COMMAND ${wxrc} -c -o ${resource_cpp} ${xrc_files}
            DEPENDS ${xrc_files}
            DEPENDS wxrc                        # Not needed with the installed version.
            COMMENT "Compiling XRC resources"
    )
    target_sources(PRToolForBitbucket2 PRIVATE ${resource_cpp})
    # END Resources
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
    )
    target_compile_options(PRToolForBitbucket2 PRIVATE
            -fno-builtin-std-forward_like
    )
    target_link_libraries(PRToolForBitbucket2 PRIVATE "-framework Cocoa")
endif ()
