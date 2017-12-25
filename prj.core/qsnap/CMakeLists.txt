project(qsnap)

find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)

set(qsnap_LIBS ${OpenCV_LIBS})

if(Qt5Gui_DIR AND Qt5Widgets_DIR)

  include_directories(${Qt5Gui_INCLUDE_DIRS})
  include_directories(${Qt5Widgets_INCLUDE_DIRS})
  set(qsnap_LIBS ${qsnap_LIBS} ${Qt5Gui_LIBRARIES} ${Qt5Widgets_LIBRARIES}
  )

else(Qt5Gui_DIR AND Qt5Widgets_DIR)

  find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)
  include(${QT_USE_FILE})
  include_directories(${QT_INCLUDES})
  set(qsnap_LIBS ${qsnap_LIBS}  ${QT_LIBRARIES}
 )

endif(Qt5Gui_DIR AND Qt5Widgets_DIR)

set(qsnap_PUBLIC_HEADERS
  qsnap.h
  qsnap_global.h
  qsnap_types.h
  qsnap_proxy.h
  qspx_impl.h
  qspx_macro.h
  qsnapx.h
)

set(qsnap_HEADERS
  ${qsnap_PUBLIC_HEADERS}
  src/eventfilters.h
  src/QCastEx.h
  src/QSnpFigure.h
  src/QSnpImageView.h
  src/QSnpSyncImageView.h
  src/QSnpImageWidget.h
  src/QSnpInstance.h
  src/QSnpListView.h
  src/QSnpListWidget.h
  src/QSnpNode.h
  src/QSnpTextEdit.h
  src/QSnpTextView.h
  src/QSnpTreeView.h
  src/QSnpTreeWidget.h
  src/QSnpPropertyWidget.h
  src/QSnpView.h
  src/QSnpToolbar.h
  src/QSnpToolbarView.h)

set(qsnap_SRCS
  src/qsnap.cpp
  src/eventfilters.cpp
  src/QSnpFigure.cpp
  src/QSnpImageView.cpp
  src/QSnpSyncImageView.cpp
  src/QSnpImageWidget.cpp
  src/QSnpInstance.cpp
  src/QSnpListView.cpp
  src/QSnpListWidget.cpp
  src/QSnpNode.cpp
  src/QSnpTextEdit.cpp
  src/QSnpTextView.cpp
  src/QSnpTreeView.cpp
  src/QSnpTreeWidget.cpp
  src/QSnpPropertyWidget.cpp
  src/QSnpView.cpp
  src/QSnpToolbar.cpp
  src/QSnpToolbarView.cpp)

add_definitions(-DQSNAP_LIB)

add_library(qsnap SHARED ${qsnap_MOC} ${qsnap_SRCS} ${qsnap_HEADERS})
set_target_properties(qsnap PROPERTIES AUTOMOC TRUE)
target_link_libraries(qsnap ${qsnap_LIBS})
