if (NOT DEFINED SOURCE_DIR OR SOURCE_DIR STREQUAL "")
    message(FATAL_ERROR "SOURCE_DIR is not defined")
endif ()

if (NOT DEFINED PATCH_FILES OR PATCH_FILES STREQUAL "")
    message(FATAL_ERROR "PATCH_FILES is not defined")
endif ()

if (NOT IS_DIRECTORY "${SOURCE_DIR}")
    message(FATAL_ERROR "Dependency source directory does not exist: ${SOURCE_DIR}")
endif ()

find_package(Git REQUIRED)
string(REPLACE "|" ";" patch_files "${PATCH_FILES}")

foreach (patch_file IN LISTS patch_files)
    if (NOT EXISTS "${patch_file}")
        message(FATAL_ERROR "Patch file does not exist: ${patch_file}")
    endif ()

    execute_process(
            COMMAND "${GIT_EXECUTABLE}" apply --check "${patch_file}"
            WORKING_DIRECTORY "${SOURCE_DIR}"
            RESULT_VARIABLE apply_check_result
            OUTPUT_QUIET
            ERROR_QUIET
    )

    if (apply_check_result EQUAL 0)
        message(STATUS "Applying patch: ${patch_file}")
        execute_process(
                COMMAND "${GIT_EXECUTABLE}" apply "${patch_file}"
                WORKING_DIRECTORY "${SOURCE_DIR}"
                RESULT_VARIABLE apply_result
                OUTPUT_VARIABLE apply_output
                ERROR_VARIABLE apply_error
        )

        if (NOT apply_result EQUAL 0)
            message(FATAL_ERROR
                    "Failed to apply patch: ${patch_file}\n"
                    "${apply_output}${apply_error}"
            )
        endif ()

        continue()
    endif ()

    execute_process(
            COMMAND "${GIT_EXECUTABLE}" apply --reverse --check "${patch_file}"
            WORKING_DIRECTORY "${SOURCE_DIR}"
            RESULT_VARIABLE reverse_check_result
            OUTPUT_QUIET
            ERROR_QUIET
    )

    if (reverse_check_result EQUAL 0)
        message(STATUS "Patch already applied: ${patch_file}")
        continue()
    endif ()

    message(FATAL_ERROR
            "Patch cannot be applied and is not already applied: ${patch_file}"
    )
endforeach ()
