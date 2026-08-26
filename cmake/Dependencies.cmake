include_guard(GLOBAL)

include(FetchContent)

function(add_dependency name)
    cmake_parse_arguments(PARSE_ARGV 1 ARG
            ""
            ""
            "PATCH_FILES"
    )

    if (ARG_KEYWORDS_MISSING_VALUES)
        message(FATAL_ERROR
                "Missing value for ${ARG_KEYWORDS_MISSING_VALUES} while declaring ${name}"
        )
    endif ()

    set(source_dir "${CMAKE_SOURCE_DIR}/.deps/${name}-src")
    set(binary_dir "${CMAKE_BINARY_DIR}/_deps/${name}-build")
    set(signature_file "${CMAKE_SOURCE_DIR}/.deps/${name}.signature")
    set(patch_args)
    set(signature_parts
            # Preserve existing source-cache signatures across the API rename.
            "declare_dependency-v1"
            "${name}"
            ${ARG_UNPARSED_ARGUMENTS}
    )

    if (ARG_PATCH_FILES)
        set(absolute_patch_files)

        foreach (patch_file IN LISTS ARG_PATCH_FILES)
            cmake_path(ABSOLUTE_PATH patch_file
                    BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                    NORMALIZE
                    OUTPUT_VARIABLE absolute_patch_file
            )

            if (absolute_patch_file MATCHES "\\|")
                message(FATAL_ERROR "Patch paths cannot contain '|': ${absolute_patch_file}")
            endif ()

            if (NOT EXISTS "${absolute_patch_file}")
                message(FATAL_ERROR "Patch file does not exist: ${absolute_patch_file}")
            endif ()

            file(SHA256 "${absolute_patch_file}" patch_hash)
            list(APPEND absolute_patch_files "${absolute_patch_file}")
            list(APPEND signature_parts "${absolute_patch_file}" "${patch_hash}")
        endforeach ()

        # Keep the patch list in one command-line argument. ApplyPatches.cmake
        # converts the separator back to a CMake list.
        string(REPLACE ";" "|" encoded_patch_files "${absolute_patch_files}")

        list(APPEND patch_args
                PATCH_COMMAND
                "${CMAKE_COMMAND}"
                "-DSOURCE_DIR=${source_dir}"
                "-DPATCH_FILES=${encoded_patch_files}"
                -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/ApplyPatches.cmake"
        )
    endif ()

    string(JOIN "\n" signature_input ${signature_parts})
    string(SHA256 dependency_signature "${signature_input}")
    string(TOUPPER "${name}" uppercase_name)
    string(MAKE_C_IDENTIFIER "${uppercase_name}" dependency_id)

    get_property(already_declared GLOBAL
            PROPERTY "_DECLARE_DEPENDENCY_${dependency_id}_SIGNATURE"
            SET
    )

    if (already_declared)
        get_property(first_signature GLOBAL
                PROPERTY "_DECLARE_DEPENDENCY_${dependency_id}_SIGNATURE"
        )

        if (NOT first_signature STREQUAL dependency_signature)
            message(FATAL_ERROR
                    "Conflicting declarations for dependency ${name}"
            )
        endif ()

        FetchContent_MakeAvailable(${name})
        return()
    endif ()

    set_property(GLOBAL
            PROPERTY "_DECLARE_DEPENDENCY_${dependency_id}_SIGNATURE"
            "${dependency_signature}"
    )

    set(source_override_variable "FETCHCONTENT_SOURCE_DIR_${uppercase_name}")
    set(has_user_source_override OFF)
    if (DEFINED ${source_override_variable}
            AND NOT "${${source_override_variable}}" STREQUAL "")
        set(has_user_source_override ON)
    endif ()

    set(reuse_existing_source OFF)
    if (NOT has_user_source_override
            AND IS_DIRECTORY "${source_dir}"
            AND EXISTS "${signature_file}")
        file(READ "${signature_file}" cached_signature)
        string(STRIP "${cached_signature}" cached_signature)

        if (cached_signature STREQUAL dependency_signature)
            set(reuse_existing_source ON)
            set("${source_override_variable}" "${source_dir}")
        endif ()
    endif ()

    if (reuse_existing_source AND ARG_PATCH_FILES)
        execute_process(
                COMMAND
                "${CMAKE_COMMAND}"
                "-DSOURCE_DIR=${source_dir}"
                "-DPATCH_FILES=${encoded_patch_files}"
                -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/ApplyPatches.cmake"
                RESULT_VARIABLE patch_result
        )

        if (NOT patch_result EQUAL 0)
            message(FATAL_ERROR "Failed to verify patches for ${name}")
        endif ()
    endif ()

    FetchContent_Declare(
            ${name}
            ${ARG_UNPARSED_ARGUMENTS}
            SOURCE_DIR "${source_dir}"
            BINARY_DIR "${binary_dir}"
            ${patch_args}
    )

    FetchContent_MakeAvailable(${name})

    if (NOT has_user_source_override)
        file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/.deps")
        file(WRITE "${signature_file}" "${dependency_signature}\n")
    endif ()
endfunction()
