add_executable(tclient client.cpp test.cpp)
add_executable(tserver server.cpp)
target_link_libraries(tclient PRIVATE thallium)
target_link_libraries(tserver PRIVATE thallium)

