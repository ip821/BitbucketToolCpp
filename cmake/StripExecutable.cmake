if (NOT CONFIGURATION MATCHES "^(Release|MinSizeRel)$")
    return()
endif ()

if (NOT EXISTS "${EXECUTABLE}")
    message(FATAL_ERROR "Executable to strip does not exist: ${EXECUTABLE}")
endif ()

execute_process(
        COMMAND "${STRIP_COMMAND}" "${EXECUTABLE}"
        RESULT_VARIABLE strip_result
        ERROR_VARIABLE strip_error
)

if (NOT strip_result EQUAL 0)
    message(FATAL_ERROR "Failed to strip ${EXECUTABLE}: ${strip_error}")
endif ()
