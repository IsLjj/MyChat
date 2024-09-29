#define variable SRC_LIST
aux_source_directory(. SRC_LIST)
aux_source_directory(./MySQL MYSQL_LIST)
#set the name of generating executable file
add_executable(ChatServer ${SRC_LIST} ${MYSQL_LIST})
#set the dynamic libfile that generating executable file depend on
target_link_libraries(ChatServer muduo_net muduo_base mysqlclient hiredis pthread)
