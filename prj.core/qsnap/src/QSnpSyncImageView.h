/**
  \file   QSnpSyncImageView.h
  \brief  QSnpSyncImageView class to show splitted source/destination image
  \author Sholomov D.
  \date   05.8.2013
*/

#pragma once
#include <qsnap/qsnap.h>

#include "QSnpView.h"
#include "QSnpFigure.h"
#include "QSnpImageWidget.h"
#include "QSnpImageView.h"

#include <QSplitter>
#include <QScrollArea>

class QSnpSyncImageView :
  public QSnpImageView
{
public:
  QSnpSyncImageView(QSnp::SyncViewOrientation orient = QSnp::SVO_HORIZONTAL);
  virtual ~QSnpSyncImageView(void);

  /// создание окна
  virtual QHandle Create(QSnpInstance* _pInstance);

  /// разрушение окна
  virtual bool Destroy();

  /// чтение свойств окна
  virtual bool loadProperties(const char* pConfig = 0);

  /// сохранение свойств окна
  virtual bool saveProperties(const char* pConfig = 0);

  /// установка изображения
  bool setSubImage(
    const char* pImageFile,             // [in] путь к файлу
    int nSubView = 0                    // [in] номер внутреннего окна показа 0..3
    );

  /// установка изображения
  bool setSubImage(
    const cv::Mat& image,               // [in] Mat-изображение
    int nSubView = 0,                   // [in] номер внутреннего окна показа 0..3
    bool bRepaint = true                // [in] перерисовка
    );

#ifdef __MINIMG__
  /// установка изображения MinImg
  bool setSubImage(
    const MinImg* pMinImage,            // [in] изображение
    int nSubView = 0                    // [in] номер внутреннего окна показа 0..3
  );
#endif

  /// добавление точки для отрисовки
  QHandle addPoint(
    int             x,                  // [in] координата по горизонтали
    int             y,                  // [in] координата по вертикали
    int             ptWidth,            // [in] толщина точки (диаметр)
    QSnp::SnpColor  color,              // [in] цвет
    const char*     idGroup             // [in] идентификатор группы фигур (для совместной работы)
    );

  /// добавление линии для отрисовки
  QHandle addLine(
    int             xFrom,              // [in] координата начала по горизонтали
    int             yFrom,              // [in] координата начала по вертикали
    int             xTo,                // [in] координата конца по горизонтали
    int             yTo,                // [in] координата конца по вертикали
    int             lineWidth,          // [in] толщина линии
    QSnp::SnpColor  color,              // [in] цвет
    QSnp::LineType  type,               // [in] тип линии (линия, пунктир и т.д.)
    const char*     idGroup
    );

  /// добавление прямоугольника для отрисовки
  QHandle addRect(
    QSnp::SnpRect*  pRect,              // [in] координаты прямоугольника
    int             lineWidth,          // [in] толщина линии
    QSnp::SnpColor  color,              // [in] цвет
    QSnp::LineType  type,               // [in] тип линии (линия, пунктир и т.д.)
    const char*     idGroup
    );

  /// добавление эллипса для отрисовки
  QHandle addEllipse(
    QSnp::SnpRect*  pRect,              // [in] координаты прямоугольника
    int             lineWidth,          // [in] толщина линии
    QSnp::SnpColor  color,              // [in] цвет
    QSnp::LineType  type,               // [in] тип линии (линия, пунктир и т.д.)
    const char*     idGroup
    );

  // получение координат пользовательской точки
  QError getUserPoint(QSnp::SnpPoint* point);

  // получение координат пользовательского прямоугольника
  QError getUserRect(QSnp::SnpRect* rect);
  
  // функция удаления всех фигур в данном окне
  QError  clearFigures();

  // получение frame-окна
  virtual QWidget* getFrameWidget();

  // показать/спрятать окно 
  virtual void showWidget(bool bShow);

  // установить тип размещения окон
  bool setLayout(
    QSnp::SyncViewOrientation& orient,    // [in] тип ориентации вертик/горизонт
    QSnp::SyncViewLayoutType& layoutType  // [in] тип размещения внутренних окон (см. SyncViewLayoutType)
    );

  // тип ориентации
  QSnp::SyncViewOrientation orientation() { return tOrient; }

  // тип лэйаута
  QSnp::SyncViewLayoutType layoutType() { return tLayoutType; }

  // возвращает внутренний виджет показа изображения
  QSnpImageWidget* getWidget(int nWidget);

  // количество виджетов
  int widgetCount() { return sizeof(scrollArea)/sizeof(QScrollArea); }

  // возвращает окно скроллирования
  QScrollArea* getScrollArea(int nArea);

public: // events

  virtual void onWheelEvent(QWheelEvent* ev);
  virtual void onHorSliderMoved(int value);
  virtual void onVerSliderMoved(int value);

protected: // members

  QSplitter splitterMain;               // основной сплиттер
  QSplitter splitter1, splitter2;       // дополнительные сплиттеры
  
  QScrollArea scrollArea[4];            // элементы скролирования окон (не более 4-х)

  QSnp::SyncViewOrientation tOrient;    // тип ориентации вертик/горизонт
  QSnp::SyncViewLayoutType tLayoutType; // тип размещения внутренних окон (см. SyncViewLayoutType)

protected: // service functions
};
