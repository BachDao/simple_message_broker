
add_executable(server server.cpp)
target_link_libraries(server PUBLIC simple_broker)

add_executable(client_1 client_1.cpp)
target_link_libraries(client_1 PUBLIC simple_broker)

add_executable(client_2 client_2.cpp)
target_link_libraries(client_2 PUBLIC simple_broker)
