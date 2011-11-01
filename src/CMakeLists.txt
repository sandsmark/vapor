set(vapor_SRCS
    vapor.cpp
    main.cpp
    mainwidget.cpp
    trackmodel.cpp
    trackview.cpp
    trackviewdelegate.cpp

   
    playpausebutton.cpp
    slider.cpp
    blockanalyzer.cpp
    analyzerbase.cpp
    
    fht.cpp

    lyricswidget.cpp


    localscanner.cpp

    collection.cpp
    
 )

kde4_add_ui_files(vapor_SRCS prefs_base.ui)

kde4_add_kcfg_files(vapor_SRCS settings.kcfgc )

kde4_add_executable(vapor ${vapor_SRCS})

target_link_libraries(vapor ${KDE4_KDEUI_LIBS} ${TAGLIB_LIBRARIES})

install(TARGETS vapor ${INSTALL_TARGETS_DEFAULT_ARGS} )

