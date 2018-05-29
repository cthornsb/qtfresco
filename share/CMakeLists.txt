#Configure and install the module file
configure_file("modulefiles/qtfresco" qtfresco @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qtfresco DESTINATION share/modulefiles/)
