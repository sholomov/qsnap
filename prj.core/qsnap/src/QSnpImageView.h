/**
  \file   QSnpImageView.h
  \brief  QSnpImageView class to show and interact with image
  \author Sholomov D.
  \date   15.5.2013
*/

#pragma once
#include <qsnap/qsnap.h>

#include "QSnpView.h"
#include "QSnpFigure.h"
#include "QSnpImageWidget.h"

#include <QScrollArea>
#include <QMap>

class QSnpImageView : public QSnpView
{
public:
  QSnpImageView(void);
  virtual ~QSnpImageView(void);

  /// создание окна
  virtual QHandle Create(QSnpInstance* _pInstance);

  /// разрушение окна
  virtual bool Destroy();

  /// чтение свойств окна
  virtual bool loadProperties(const char* pConfig = 0);

  /// сохранение свойств окна
  virtual bool saveProperties(const char* pConfig = 0);

  /// установка изображения
  bool setImage(const char* pImageFile, bool bRepaint = true);

  /// установка изображения cv::Mat
  bool setImage(const cv::Mat& image, bool bRepaint = true);

#ifdef __MINIMG__
  /// установка изображения MinImg
  bool setImage(const MinImg *pMinImage, bool bRepaint = true);
#endif

  /// элемент скроллирования
  QScrollArea* getScrollArea() { return &scrollArea; } 

  /// установка коэффициента масштабирования
  QError setScaleFactor(
    double          scale,              // [in] коэффициент масштабирования
    const char*     idGroup = 0         // [in] идентификатор группы фигур (для совместной работы)
    );

  /// получение коэффициента масштабирования
  double getScaleFactor(
    const char*     idGroup = 0         // [in] идентификатор группы фигур (для совместной работы)
    );

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
    const char*     idGrouphView
    );

  /// добавление прямоугольника для отрисовки
  QHandle addRect(
    QSnp::SnpRect*  pRect,              // [in] координаты прямоугольника
    int             lineWidth,          // [in] толщина линии
    QSnp::SnpColor  color,              // [in] цвет
    QSnp::LineType  type,               // [in] тип линии (линия, пунктир и т.д.)
    const char*     idGrouphView
    );

  /// добавление эллипса для отрисовки
  QHandle addEllipse(
    QSnp::SnpRect*     pRect,              // [in] координаты прямоугольника
    int             lineWidth,          // [in] толщина линии
    QSnp::SnpColor  color,              // [in] цвет
    QSnp::LineType  type,               // [in] тип линии (линия, пунктир и т.д.)
    const char*     idGrouphView
    );

  /// добавление текстового значения
  QHandle addText(
    QSnp::SnpRect*  pRect,              // [in] координаты охватывающего прямоугольника
    const char*     textValue,          // [in] текстовое значение
    int             fontSize,           // [in] кегль шрифта
    QSnp::SnpColor  fontColor,          // [in] цвет шрифта
    const char*     fontType,           // [in] тип шрифта
    const char*     idGroup             // [in] идентификатор группы фигур
    );

  // получение координат пользовательской точки
  QError getUserPoint(QSnp::SnpPoint* point);

  // получение координат пользовательского прямоугольника
  QError getUserRect(QSnp::SnpRect* rect);
  
  // функция удаления всех фигур в данном окне
  QError  clearFigures();

  // получение frame-окна
  virtual QWidget* getFrameWidget() { return getScrollArea(); }

  // показать/спрятать окно 
  virtual void showWidget(bool bShow);

public: // events

  virtual void onWheelEvent(QWheelEvent* ev);
  virtual void onHorSliderMoved(int value);
  virtual void onVerSliderMoved(int value);

public: // members
  QList<QSnpFigure*> lsFigures;         // массив фигур для прорисовки
protected: // members
  QScrollArea scrollArea;               // элемент скролирования
  
  int nStartHorSliderPos;
  int nStartVerSliderPos;

  QMap<QString,double> scaleFactors;    // массив масштабов для каждой группы фигур


};
