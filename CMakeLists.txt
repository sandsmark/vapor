project(vapor)

find_package(KDE4 REQUIRED)
include (KDE4Defaults)

set(TAGLIB_MIN_VERSION "1.6")
find_package(Taglib REQUIRED)

include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

#add_subdirectory( doc )
add_subdirectory( src )
add_subdirectory( icons )

add_subdirectory(images)


########### install files ###############

install( PROGRAMS vapor.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
install( FILES src/vapor.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES src/vaporui.rc  DESTINATION  ${DATA_INSTALL_DIR}/vapor )

install(FILES 
              images/media-playback-pause.png
              images/media-playback-start.png
              images/slider_left.png
              images/slider_right.png
              images/slider_body_background.png
              images/slider_body_left.png
              images/slider_body_right.png
              images/slider_body.png
              images/dialog-information.png
              images/searching.png DESTINATION ${DATA_INSTALL_DIR}/vapor/images)
