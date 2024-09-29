aux_source_directory(. SRCLISTS)
add_executable(ChatClient ${SRCLISTS})
target_link_libraries(ChatClient pthread)