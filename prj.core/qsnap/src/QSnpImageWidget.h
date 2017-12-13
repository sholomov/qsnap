/**
  \file   QSnpImageWidget.h
  \brief  QWidget based class to show and interact with image
  \author Sholomov D.
  \date   15.5.2013
*/

#pragma once

#include "QSnpFigure.h"
#include "QSnpImageView.h"

#include <QScrollArea>

class QSnpImageView;

class QSnpImageWidget :
  public QWidget
{
  Q_OBJECT
public:
  QSnpImageWidget(QSnpImageView* pView = NULL);
  virtual ~QSnpImageWidget(void);

  void setFigures(QList<QSnpFigure*>* pf) { plsFigures = pf; }

  // информация о пользовательском прямоугольнике
  bool    userRectSet()   { return bUserRectSet; }
  QRect   userRect()      { return rcUserRect; }
  
  // информация о пользовательской точке
  bool    userPointSet()  { return bUserPointSet; }
  QPoint  userPoint()     { return ptUserPoint; }

  QSnpImageView* getView() { return pImageView; }
  QSnpImageView* getParentView() { return pParentImageView; }
  void setParentView(QSnpImageView* pView) { pParentImageView = pView; }
  
protected: // events
  virtual void paintEvent(QPaintEvent* ev);
  virtual void mousePressEvent (QMouseEvent* ev);
  virtual void mouseMoveEvent (QMouseEvent* ev);
  virtual void mouseReleaseEvent(QMouseEvent* ev);
  virtual void wheelEvent(QWheelEvent* ev);

protected slots:
  virtual void onHorSliderMoved ( int value );
  virtual void onVerSliderMoved ( int value );
  virtual void onWheelEvent(QWheelEvent* ev);

protected: // members
  QImage  image;                         ///< изображение
  float   ratio;                         ///< коэффициент сжатия изображения 
  QList<QSnpFigure*>* plsFigures;

  // отображение пользовательского прямоугольника
  QRect   rcUserRect;
  bool    bUserRectSet;

  // отображение пользовательской точки
  QPoint  ptUserPoint;
  bool    bUserPointSet;

  bool    bMouseIsDragging;
  bool    bPictureDragging; // перетаскивание картинки
  QPoint  ptFirstDragPoint;
  QPoint  ptSecondDragPoint;
  QPoint  ptFirstSliderPos;

  QCursor  cursor;
  QPainter painter;

public:
  Qt::KeyboardModifiers kbdModifiers; // последние модификаторы нажатия клавиши

  friend class QSnpImageView;
  friend class QSnpSyncImageView;
  QSnpImageView* pImageView;
  QSnpImageView* pParentImageView;
  QScrollArea* pScrollArea;

};
