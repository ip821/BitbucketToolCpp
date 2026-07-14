cmake_minimum_required(VERSION 4.0)
project(bitbucket_api LANGUAGES C CXX)
include(FetchContent)

FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.12.0
)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
        cpp-curl
        GIT_REPOSITORY https://github.com/ip821/cpp-curl.git
)
FetchContent_MakeAvailable(cpp-curl)

FetchContent_Declare(
        cpp-utils
        GIT_REPOSITORY https://github.com/ip821/cpp-utils.git
)
FetchContent_MakeAvailable(cpp-utils)

add_library(bitbucket_api
        include/bitbucket_api/BitbucketResponse.h
        include/bitbucket_api/ParticipantUser.h
        include/bitbucket_api/PullRequest.h
        include/bitbucket_api/Requests.h
        include/bitbucket_api/Status.h
        include/bitbucket_api/Values.h
        include/bitbucket_api/Structs.h
        include/bitbucket_api/BitbucketApiGlobalInit.h
        include/bitbucket_api/StructsSerialization.h
        src/PullRequest.cpp
        src/Strings.h
        src/Structs.cpp
        src/BitbucketRequest.h
        src/BitbucketUrlBuilder.h
        src/Requests.cpp
        src/ServerResponse.h
        src/BitbucketApiGlobalInit.cpp
)

add_library(bitbucket_api::bitbucket_api ALIAS bitbucket_api)
target_compile_features(bitbucket_api PUBLIC cxx_std_23)
target_include_directories(bitbucket_api
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(bitbucket_api PRIVATE
        cpp_curl::cpp_curl
        cpp_utils::cpp_utils
        nlohmann_json::nlohmann_json
)
