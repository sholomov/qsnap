/**
  \file   QSnpSyncImageView.cpp
  \brief  Class functions for QSnpImageView class functions to show and interact with image
  \author Sholomov D.
  \date   15.5.2013
*/

#include "QSnpSyncImageView.h"
#include "QSnpImageWidget.h"

#include <QScrollBar>
#include <QSettings>

#include "opencv2/highgui/highgui.hpp"

using namespace QSnp;

QSnpSyncImageView::QSnpSyncImageView(SyncViewOrientation orient) : 
  splitterMain(orient==SVO_VERTICAL ? Qt::Vertical : Qt::Horizontal)
{
  pWidget = NULL;
  eViewType = VT_SYNC_IMAGE_VIEW;
  tOrient = orient;
  tLayoutType = SVL_DEFAULT;
}

QSnpSyncImageView::~QSnpSyncImageView(void)
{
  Destroy();
}

QHandle QSnpSyncImageView::Create(QSnpInstance* _pInstance)
{
  if(pWidget)
    return (QHandle)this;
  
  if(tLayoutType != SVL_DEFAULT)
    return QHANDLE_INVALID;

  QSnpImageWidget* pWidget1 = new QSnpImageWidget(this);
  pWidget1->setFigures(&lsFigures);

  QSnpImageWidget* pWidget2 = new QSnpImageWidget(this);
  pWidget2->setFigures(&lsFigures);

  scrollArea[0].setWidget(pWidget1);
  scrollArea[1].setWidget(pWidget2);

  //scrollArea[0].horizontalScrollBar()->setMaximum(scrollArea[0].horizontalScrollBar()->maximum()/2);
  //scrollArea[0].verticalScrollBar()->setMaximum(scrollArea[0].verticalScrollBar()->maximum()/2);

  pWidget1->pScrollArea = &scrollArea[0];
  pWidget2->pScrollArea = &scrollArea[1];

  splitterMain.setOrientation(Qt::Vertical);//tOrient == SVO_HORIZONTAL ? Qt::Horizontal : Qt::Vertical);
  splitterMain.addWidget(&scrollArea[0]);
  splitterMain.addWidget(&scrollArea[1]);

  scrollArea[0].connect(
    scrollArea[0].horizontalScrollBar(), SIGNAL(valueChanged(int)),
    pWidget1, SLOT(onHorSliderMoved(int))
    );
  scrollArea[0].connect(
    scrollArea[0].verticalScrollBar(), SIGNAL(valueChanged(int)),
    pWidget2, SLOT(onVerSliderMoved(int))
    );
  scrollArea[1].connect(
    scrollArea[1].horizontalScrollBar(), SIGNAL(valueChanged(int)),
    pWidget1, SLOT(onHorSliderMoved(int))
    );
  scrollArea[1].connect(
    scrollArea[1].verticalScrollBar(), SIGNAL(valueChanged(int)),
    pWidget2, SLOT(onVerSliderMoved(int))
    );

  splitterMain.show();
  pWidget1->show();
  pWidget2->show();
  scrollArea[0].show();
  scrollArea[1].show();

  QSnpView::pWidget = NULL;//(QWidget*)1;
  pWidget1->update();
  pWidget2->update();
  
  pWidget1->setParentView(this);
  pWidget2->setParentView(this);

  return QHANDLE_INVALID;
}

bool QSnpSyncImageView::Destroy(void)
{
  foreach(QSnpFigure* pf, lsFigures)
    delete pf;
  lsFigures.clear();

  if(!pWidget)
    return true;
  delete pWidget;
  pWidget = 0;
  return true;
}

bool QSnpSyncImageView::loadProperties(const char* _pConfig)
{
  const char* pConfig = _pConfig ? _pConfig : sConfigDefault.c_str();

  LoadWindowPos(getFrameWidget(), pConfig);

  QSettings sett("QSnap");
  sett.beginGroup(pConfig);

  float _ratio = sett.value(QString("/")+=QString(pConfig)+="Ratio").toFloat();
  if(abs(_ratio)<0.0001)
    _ratio = 1.0;
  int splitPos = 0;

  // splitter
  splitterMain.restoreState(sett.value("splitterSizes").toByteArray());

  // sliders
  nStartHorSliderPos = sett.value(QString("/")+=QString(pConfig)+="HorSliderPos").toInt();
  nStartVerSliderPos = sett.value(QString("/")+=QString(pConfig)+="VerSliderPos").toInt();
  
  QSnpImageWidget* pw = NULL;
  pw = (QSnpImageWidget*)getWidget(0);
  if(pw)
    pw->ratio = _ratio;
  pw = (QSnpImageWidget*)getWidget(1);
  if(pw)
    pw->ratio = _ratio;

  return true;
}

bool QSnpSyncImageView::saveProperties(const char* _pConfig)
{
  const char* pConfig = _pConfig ? _pConfig : sConfigDefault.c_str();

  SaveWindowPos(getFrameWidget(), pConfig);


  QSnpImageWidget* pw = (QSnpImageWidget*)getWidget(0);
  QScrollArea* sa = getScrollArea(0);
  if(!pw || !sa)
    return false;

  QSettings sett("QSnap");
  sett.beginGroup(pConfig);
  
  // splitter
  sett.setValue("splitterSizes", splitterMain.saveState());
  // ratio
  sett.setValue(QString("/")+=QString(pConfig)+="Ratio", pw->ratio);
  // sliders
  sett.setValue(QString("/")+=QString(pConfig)+="HorSliderPos", sa->horizontalScrollBar()->sliderPosition());
  sett.setValue(QString("/")+=QString(pConfig)+="VerSliderPos", sa->verticalScrollBar()->sliderPosition());

  sett.endGroup();

  return true;
}

/// установка изображения
bool QSnpSyncImageView::setSubImage(const char* pImageFile, int nSubView)
{
  QSnpImageWidget* pw = (QSnpImageWidget*)getWidget(nSubView);

  pw->image.load(pImageFile);
  pw->resize(pw->image.width() * pw->ratio, pw->image.height() * pw->ratio);

  onHorSliderMoved(nStartHorSliderPos);
  onVerSliderMoved(nStartVerSliderPos);

  pw->repaint();

  return true;
}

/// установка изображения
bool QSnpSyncImageView::setSubImage(const cv::Mat& image, int nSubView, bool bRepaint)
{
  QSnpImageWidget* pw = (QSnpImageWidget*)getWidget(nSubView);

  if (image.empty())
    return false;

  bool imageWasEmpty = pw->image.isNull();

  if (image.channels()==3) // fork for different image types
    pw->image = QImage( (uchar*)image.data, image.cols, image.rows, static_cast<int> (image.step), QImage::Format_RGB888).rgbSwapped();
  else if (image.channels()==1)
  {
    // TODO: show grayscale image correctly
    pw->image = QImage();
  }
  else
    pw->image = QImage();

  //loadProperties();

  splitterMain.setOrientation(Qt::Vertical);//tOrient == SVO_HORIZONTAL ? Qt::Horizontal : Qt::Vertical);

  if(imageWasEmpty)
  {
    pw->resize(pw->image.width() * pw->ratio, pw->image.height() * pw->ratio);
    onHorSliderMoved(nStartHorSliderPos);
    onVerSliderMoved(nStartVerSliderPos);
  }

  if(bRepaint)
    pw->repaint();

  return true;
}

#ifdef __MINIMG__

/// установка изображения MinImg
bool QSnpSyncImageView::setSubImage(const MinImg* pMinImage, int nSubView)
{
  QSnpImageWidget* pw = (QSnpImageWidget*)getWidget(nSubView);

  if (pMinImage == 0 || pMinImage->pScan0 == 0)
    return false;

  if (pMinImage->format == FMT_UINT && pMinImage->channelDepth == 1 && pMinImage->channels == 1)
  {
    pw->image = QImage(pMinImage->pScan0, pMinImage->width, 
      pMinImage->height, pMinImage->stride, QImage::Format_Indexed8);
  }
  else if (pMinImage->format == FMT_UINT && pMinImage->channelDepth == 1 && pMinImage->channels == 3)
  {
    pw->image = QImage(pMinImage->pScan0, pMinImage->width, 
      pMinImage->height, pMinImage->stride, QImage::Format_RGB888);
  }
  else
  {
    pw->image = QImage();
  }

  pw->resize(pw->image.width() * pw->ratio, pw->image.height() * pw->ratio);

  onHorSliderMoved(nStartHorSliderPos);
  onVerSliderMoved(nStartVerSliderPos);

  pw->repaint();

  return true;
}

#endif

QHandle QSnpSyncImageView::addPoint(
    int           x,                  // [in] координата по горизонтали
    int           y,                  // [in] координата по вертикали
    int           ptWidth,            // [in] толщина точки (диаметр)
    SnpColor      color,              // [in] цвет
    const char*   idGroup             // [in] идентификатор группы фигур (для совместной работы)
    )
{
  QSnpFigure* pSnpFigure = new QSnpPoint(this, x, y, ptWidth);
  pSnpFigure->color = QColor_cast(color);
  lsFigures.push_back(pSnpFigure);

  return (QHandle)pSnpFigure;
}

QHandle QSnpSyncImageView::addLine(
    int             xFrom,              // [in] координата начала по горизонтали
    int             yFrom,              // [in] координата начала по вертикали
    int             xTo,                // [in] координата конца по горизонтали
    int             yTo,                // [in] координата конца по вертикали
    int             lineWidth,          // [in] толщина линии
    QSnp::SnpColor  color,              // [in] цвет
    QSnp::LineType  type,               // [in] тип линии (линия, пунктир и т.д.)
    const char*     idGrouphView
    )
{
  QSnpLine* pSnpFigure = new QSnpLine(this);
  pSnpFigure->ptFrom = QPoint(xFrom, yFrom);
  pSnpFigure->ptTo = QPoint(xTo, yTo);
  pSnpFigure->lineWidth = lineWidth;
  pSnpFigure->color = QColor_cast(color);
  pSnpFigure->rc = QRect(xFrom,yFrom,1,1).united(QRect(xTo,yTo,1,1)); // охватывающий прямоугольник
  lsFigures.push_back(pSnpFigure);

  return (QHandle)pSnpFigure;
}

QHandle QSnpSyncImageView::addRect(
    QSnp::SnpRect*  pRect,              // [in] координаты прямоугольника
    int             lineWidth,          // [in] толщина линии
    QSnp::SnpColor  color,              // [in] цвет
    QSnp::LineType  type,               // [in] тип линии (линия, пунктир и т.д.)
    const char*     idGrouphView
    )
{
  QSnpFigure* pSnpFigure = new QSnpRect(this);
  pSnpFigure->rc = QRect(pRect->x, pRect->y, pRect->width, pRect->height);
  pSnpFigure->lineWidth = lineWidth;
  pSnpFigure->color = QColor_cast(color);
  lsFigures.push_back(pSnpFigure);
  
  return (QHandle)pSnpFigure;
}

QHandle QSnpSyncImageView::addEllipse(
    QSnp::SnpRect*  pRect,              // [in] координаты прямоугольника
    int             lineWidth,          // [in] толщина линии
    QSnp::SnpColor  color,              // [in] цвет
    QSnp::LineType  type,               // [in] тип линии (линия, пунктир и т.д.)
    const char*     idGrouphView
    )
{
  QSnpFigure* pSnpFigure = new QSnpEllipse(this);
  pSnpFigure->rc = QRect(pRect->x, pRect->y, pRect->width, pRect->height);
  pSnpFigure->lineWidth = lineWidth;
  pSnpFigure->color = QColor_cast(color);
  lsFigures.push_back(pSnpFigure);
  
  return (QHandle)pSnpFigure;
}

// получение координат пользовательской точки
QError QSnpSyncImageView::getUserPoint(QSnp::SnpPoint* point)
{
  QSnpImageWidget* pw = (QSnpImageWidget*)pWidget;
  if(!pw)
      return QERR_ERROR;
  if(!pw->userPointSet())
      return QERR_ERROR;

  QPoint pt = pw->userPoint();
  point->x = pt.x();
  point->y = pt.y();

  return QERR_NO_ERROR;
}

// получение координат пользовательского прямоугольника
QError QSnpSyncImageView::getUserRect(QSnp::SnpRect* rect)
{
  QSnpImageWidget* pw = (QSnpImageWidget*)pWidget;
  if(!pw)
      return QERR_ERROR;
  if(!pw->userRectSet())
      return QERR_ERROR;

  QRect rc = pw->userRect();
  rect->x = rc.x();
  rect->y = rc.y();
  rect->width = rc.width();
  rect->height = rc.height();

  return QERR_NO_ERROR;
}

// функция удаления всех фигур в данном окне
QError  QSnpSyncImageView::clearFigures()
{
  foreach(QSnpFigure* pf, lsFigures)
    delete pf;

  lsFigures.clear();
  getWidget(0)->update();

  return QERR_NO_ERROR;
}

// получение frame-окна
QWidget* QSnpSyncImageView::getFrameWidget()
{
  return &splitterMain;
}

void QSnpSyncImageView::showWidget(bool bShow)
{
  QSnpImageWidget* pw = (QSnpImageWidget*)pWidget;
  if(bShow) 
    scrollArea[0].show();
  else 
    scrollArea[0].hide();
}


bool QSnpSyncImageView::setLayout(SyncViewOrientation& orient, SyncViewLayoutType& layoutType)
{
  return true;
}

// возвращает внутренний виджет показа изображения
QSnpImageWidget* QSnpSyncImageView::getWidget(int nWidget)
{
  if(nWidget >=widgetCount())
    return NULL;

  QScrollArea& sa = scrollArea[nWidget];
  return (QSnpImageWidget*)sa.widget();
}

// возвращает окно скроллирования
QScrollArea* QSnpSyncImageView::getScrollArea(int nArea)
{
  if(nArea >=widgetCount())
    return NULL;

  QScrollArea& sa = scrollArea[nArea];
  return &sa;
}

void QSnpSyncImageView::onWheelEvent(QWheelEvent* ev)
{
  for(int nWidget = 0; nWidget<widgetCount(); nWidget++)
  {
    QSnpImageWidget* pw = getWidget(nWidget);
    if(!pw)
      continue;
    pw->onWheelEvent(ev);
  }
}

void QSnpSyncImageView::onHorSliderMoved ( int value )
{
  for(int nWidget = 0; nWidget<widgetCount(); nWidget++)
  {
    QSnpImageWidget* pw = getWidget(nWidget);
    if(!pw)
      continue;
    QScrollArea& sa = scrollArea[nWidget];            // элементы скролирования окон (не более 4-х)
    sa.horizontalScrollBar()->setSliderPosition(value);
  }
}

void QSnpSyncImageView::onVerSliderMoved ( int value )
{
  for(int nWidget = 0; nWidget<widgetCount(); nWidget++)
  {
    QSnpImageWidget* pw = getWidget(nWidget);
    if(!pw)
      continue;
    QScrollArea& sa = scrollArea[nWidget];            // элементы скролирования окон (не более 4-х)
    sa.verticalScrollBar()->setSliderPosition(value);
  }
}

