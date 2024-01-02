add_library(${LIBRARY_NAME} STATIC 
	util/graph.cpp
	util/topologies.cpp

	net/net.cpp
)

target_include_directories(${LIBRARY_NAME}
	PUBLIC "util/include"
	PUBLIC "net/include"
)

#[[
target_link_libraries(${LIBRARY_NAME}
	PUBLIC cxxopts::cxxopts
	PUBLIC nlohmann_json::nlohmann_json
	PUBLIC fmt::fmt
	PUBLIC spdlog::spdlog	
)
]]
