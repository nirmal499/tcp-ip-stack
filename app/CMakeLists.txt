add_executable(${EXECUTABLE_NAME} main.cpp)

target_link_libraries(${EXECUTABLE_NAME}
	PUBLIC ${LIBRARY_NAME}
	PRIVATE cli::cli
)

#[[
target_link_libraries(${EXECUTABLE_NAME}
	PUBLIC ${LIBRARY_NAME}
	PUBLIC nlohmann_json
	PUBLIC some_log)
]]