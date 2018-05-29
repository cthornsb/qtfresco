set(QT_DEFINES "-DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED")
set(QT_INCPATH "-I/usr/include/qt4 -I/usr/include/qt4/QtGui")

set(QTFRESCO_HEADER ${TOP_DIRECTORY}/include/qtfresco.hpp)
set(QTFRESCO_UI_HEADER ${TOP_DIRECTORY}/qtfiles/qtfresco.ui)
set(MOC_FILENAME ${CMAKE_CURRENT_BINARY_DIR}/moc_qtfresco.cpp)
set(UIC_FILENAME ${CMAKE_CURRENT_BINARY_DIR}/ui_qtfresco.hpp)

#Use moc to generate the moc_qtfresco source file.
add_custom_target(MocFile ALL DEPENDS ${MOC_FILENAME})
add_custom_command(OUTPUT ${MOC_FILENAME} COMMAND ${MOC_EXECUTABLE} ${QT_DEFINES} ${QT_INCPATH} ${QTFRESCO_HEADER} -o ${MOC_FILENAME} DEPENDS ${QTFRESCO_HEADER} VERBATIM)

#Use uic to generate the ui_qtfresco header file.
add_custom_target(UicFile ALL DEPENDS ${UIC_FILENAME})
add_custom_command(OUTPUT ${UIC_FILENAME} COMMAND ${UIC_EXECUTABLE} ${QTFRESCO_UI_HEADER} -o ${UIC_FILENAME} DEPENDS ${QTFRESCO_UI_HEADER} VERBATIM)

#Build qtfresco executable.
add_executable(qtfresco main.cpp qtfresco.cpp ${MOC_FILENAME} graphPlotter.cpp frescout.cpp)
target_include_directories(qtfresco PUBLIC ${QT_INC_DIR} ${QT_GUI_INC_DIR} ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(qtfresco ${QT_GUI_LIB} ${QT_CORE_LIB} ${SimpleScan_OPT_LIB} ${ROOT_LIBRARIES})
install(TARGETS qtfresco DESTINATION bin)
