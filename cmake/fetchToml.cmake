macro(fetchTomlInit TOML_PATH)
    find_program(TOML_CLI_BIN toml-cli toml OPTIONAL) # I have a fallback
    if(TOML_CLI_BIN)
        macro(fetchToml QUERY OUTVAR)
            execute_process(
                COMMAND ${TOML_CLI_BIN} get --raw ${TOML_PATH} ${QUERY}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                OUTPUT_VARIABLE ${OUTVAR}
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
        endmacro()
    else()
        find_program(YQ_BIN yq REQUIRED)
        macro(fetchToml QUERY OUTVAR)
            execute_process(
                COMMAND ${YQ_BIN} ".${QUERY}" ${TOML_PATH}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                OUTPUT_VARIABLE ${OUTVAR}
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
        endmacro()
    endif()
endmacro()