set(vapor_SRCS
   vapor.cpp
   main.cpp
   vaporview.cpp
 )

kde4_add_ui_files(vapor_SRCS vaporview_base.ui prefs_base.ui)

kde4_add_kcfg_files(vapor_SRCS settings.kcfgc )

kde4_add_executable(vapor ${vapor_SRCS})

target_link_libraries(vapor ${KDE4_KDEUI_LIBS} )

install(TARGETS vapor ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( PROGRAMS vapor.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
install( FILES vapor.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES vaporui.rc  DESTINATION  ${DATA_INSTALL_DIR}/vapor )
