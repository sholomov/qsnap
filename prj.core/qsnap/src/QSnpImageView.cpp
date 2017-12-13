/**
  \file   QSnpImageView.cpp
  \brief  Class functions for QSnpImageView class functions to show and interact with image
  \author Sholomov D.
  \date   15.5.2013
*/

#include "QSnpImageView.h"
#include "QSnpImageWidget.h"

#include <QSettings>
#include <QScrollBar>

using namespace QSnp;
using namespace std;

QSnpImageView::QSnpImageView(void)
{
  pWidget = NULL;
  eViewType = VT_IMAGE_VIEW;
  nStartHorSliderPos = 0;
  nStartVerSliderPos = 0;
  scaleFactors[""]=1.0;
}

QSnpImageView::~QSnpImageView(void)
{
  Destroy();
}

QHandle QSnpImageView::Create(QSnpInstance* _pInstance)
{
  if(pWidget)
    return (QHandle)this;
  
  QSnpView::Create(_pInstance);

  QSnpImageWidget* pWidget = new QSnpImageWidget(this);
  pWidget->setFigures(&lsFigures);

  scrollArea.setWidget(pWidget);
  pWidget->pScrollArea = &scrollArea;

  scrollArea.connect(
    scrollArea.horizontalScrollBar(), SIGNAL(valueChanged(int)),
    pWidget, SLOT(onHorSliderMoved(int))
    );
  scrollArea.connect(
    scrollArea.verticalScrollBar(), SIGNAL(valueChanged(int)),
    pWidget, SLOT(onVerSliderMoved(int))
    );

  scrollArea.show();
  pWidget->show();

  QSnpView::pWidget = pWidget;
  pWidget->update();
  return QHANDLE_INVALID;
}

bool QSnpImageView::Destroy(void)
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

bool QSnpImageView::loadProperties(const char* _pConfig)
{
  const char* pConfig = _pConfig ? _pConfig : sConfigDefault.c_str();

  LoadWindowPos(getScrollArea(), pConfig);

  QSettings sett("QSnap");
  sett.beginGroup(pConfig);
  float _ratio = sett.value(QString("/")+=QString(pConfig)+="Ratio").toFloat();
  if(abs(_ratio)<0.0001)
    _ratio = 1.0;

   // sliders
  nStartHorSliderPos = sett.value(QString("/")+=QString(pConfig)+="HorSliderPos").toInt();
  nStartVerSliderPos = sett.value(QString("/")+=QString(pConfig)+="VerSliderPos").toInt();

  sett.endGroup();
  
  QSnpImageWidget* pw = (QSnpImageWidget*)pWidget;
  if(pw)
    pw->ratio = _ratio;
  return true;
}

bool QSnpImageView::saveProperties(const char* _pConfig)
{
  const char* pConfig = _pConfig ? _pConfig : sConfigDefault.c_str();

  SaveWindowPos(getScrollArea(), pConfig);

  QSnpImageWidget* pw = (QSnpImageWidget*)pWidget;
  if(!pw)
    return false;

  QSettings sett("QSnap");
  sett.beginGroup(pConfig);
  
  // ratio
  sett.setValue(QString("/")+=QString(pConfig)+="Ratio", pw->ratio);
  // sliders
  sett.setValue(QString("/")+=QString(pConfig)+="HorSliderPos", scrollArea.horizontalScrollBar()->sliderPosition());
  sett.setValue(QString("/")+=QString(pConfig)+="VerSliderPos", scrollArea.verticalScrollBar()->sliderPosition());

  sett.endGroup();

  return true;
}

/// установка изображения
bool QSnpImageView::setImage(const char* pImageFile, bool bRepaint)
{
  QSnpImageWidget* pw = (QSnpImageWidget*)pWidget;
  
  pw->image.load(pImageFile);
  pw->resize(pw->image.width() * pw->ratio, pw->image.height() * pw->ratio);

  onHorSliderMoved(nStartHorSliderPos);
  onVerSliderMoved(nStartVerSliderPos);

  if(bRepaint)
    pWidget->repaint();

  return true;
}

/// установка изображения
bool QSnpImageView::setImage(const cv::Mat& image, bool bRepaint)
{
  QSnpImageWidget* pw = (QSnpImageWidget*)pWidget;
  
  if (image.empty())
    return false;

  if (image.channels()==3) // fork for different image types
    pw->image = QImage( (uchar*)image.data, image.cols, image.rows, static_cast<int>(image.step), QImage::Format_RGB888).rgbSwapped();
  else
    pw->image = QImage();

  loadProperties();

  pw->resize(pw->image.width() * pw->ratio, pw->image.height() * pw->ratio);

  onHorSliderMoved(nStartHorSliderPos);
  onVerSliderMoved(nStartVerSliderPos);

  if(bRepaint)
    pWidget->repaint();

  return true;
}

#ifdef __MINIMG__

bool QSnpImageView::setImage(const MinImg *pMinImage, bool bRepaint)
{
  QSnpImageWidget* pw = (QSnpImageWidget*)pWidget;

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

  // onHorSliderMoved(nStartHorSliderPos);
  // onVerSliderMoved(nStartVerSliderPos);

  if(bRepaint)
    pWidget->repaint();

  return true;
}

#endif

QError QSnpImageView::setScaleFactor(
    double          scale,              // [in] коэффициент масштабирования
    const char*     idGroup             // [in] идентификатор группы фигур (для совместной работы)
    )
{
  scaleFactors[idGroup ? idGroup : ""] = scale;
  return QERR_NO_ERROR;
}

double QSnpImageView::getScaleFactor(
    const char*     idGroup             // [in] идентификатор группы фигур (для совместной работы)
    )
{
  QMap<QString,double>::iterator it = scaleFactors.find(idGroup);
  if(it==scaleFactors.end())
    return scaleFactors[""];
  return *it;
}

QHandle QSnpImageView::addPoint(
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

QHandle QSnpImageView::addLine(
    int             xFrom,              // [in] координата начала по горизонтали
    int             yFrom,              // [in] координата начала по вертикали
    int             xTo,                // [in] координата конца по горизонтали
    int             yTo,                // [in] координата конца по вертикали
    int             lineWidth,          // [in] толщина линии
    QSnp::SnpColor  color,              // [in] цвет
    QSnp::LineType  type,               // [in] тип линии (линия, пунктир и т.д.)
    const char*     idGroup
    )
{
  QSnpLine* pSnpFigure = new QSnpLine(this);

  double scale = getScaleFactor(idGroup); // масштаб для данной группы
  pSnpFigure->ptFrom = QPoint(xFrom*scale, yFrom*scale);
  pSnpFigure->ptTo = QPoint(xTo*scale, yTo*scale);
  pSnpFigure->lineWidth = lineWidth;
  pSnpFigure->color = QColor_cast(color);
  pSnpFigure->rc = QRect(xFrom,yFrom,1,1).united(QRect(xTo,yTo,1,1)); // охватывающий прямоугольник
  lsFigures.push_back(pSnpFigure);

  return (QHandle)pSnpFigure;
}

QHandle QSnpImageView::addRect(
    QSnp::SnpRect*  pRect,              // [in] координаты прямоугольника
    int             lineWidth,          // [in] толщина линии
    QSnp::SnpColor  color,              // [in] цвет
    QSnp::LineType  type,               // [in] тип линии (линия, пунктир и т.д.)
    const char*     idGroup
    )
{
  QSnpFigure* pSnpFigure = new QSnpRect(this);

  double scale = getScaleFactor(idGroup); // масштаб для данной группы
  QRect qRect(pRect->x*scale, pRect->y*scale, pRect->width*scale, pRect->height*scale);
  pSnpFigure->rc = qRect;
  pSnpFigure->lineWidth = lineWidth;
  pSnpFigure->color = QColor_cast(color);
  lsFigures.push_back(pSnpFigure);
  
  return (QHandle)pSnpFigure;
}

QHandle QSnpImageView::addEllipse(
    QSnp::SnpRect*  pRect,              // [in] координаты прямоугольника
    int             lineWidth,          // [in] толщина линии
    QSnp::SnpColor  color,              // [in] цвет
    QSnp::LineType  type,               // [in] тип линии (линия, пунктир и т.д.)
    const char*     idGroup
    )
{
  QSnpFigure* pSnpFigure = new QSnpEllipse(this);

  double scale = getScaleFactor(idGroup); // масштаб для данной группы
  QRect qRect(pRect->x*scale, pRect->y*scale, pRect->width*scale, pRect->height*scale);

  pSnpFigure->rc = qRect;
  pSnpFigure->lineWidth = lineWidth;
  pSnpFigure->color = QColor_cast(color);
  lsFigures.push_back(pSnpFigure);
  
  return (QHandle)pSnpFigure;
}

QHandle QSnpImageView::addText(
    QSnp::SnpRect*  pRect,              // [in] координаты охватывающего прямоугольника
    const char*     textValue,          // [in] текстовое значение
    int             fontSize,           // [in] кегль шрифта
    QSnp::SnpColor  fontColor,          // [in] цвет шрифта
    const char*     fontType,           // [in] тип шрифта
    const char*     idGroup             // [in] идентификатор группы фигур
    )
{
  QSnpText* pSnpFigure = new QSnpText(this);

  double scale = getScaleFactor(idGroup); // масштаб для данной группы
  QRect qRect(pRect->x*scale, pRect->y*scale, pRect->width*scale, pRect->height*scale);

  pSnpFigure->rc = qRect;
  pSnpFigure->lineWidth = 1;
  pSnpFigure->color = QColor_cast(fontColor);

  pSnpFigure->textValue = textValue;
  pSnpFigure->fontSize = fontSize;
  pSnpFigure->fontType = fontType;

  lsFigures.push_back(pSnpFigure);
  
  return (QHandle)pSnpFigure;
}

// получение координат пользовательской точки
QError QSnpImageView::getUserPoint(QSnp::SnpPoint* point)
{
  QSnpImageWidget* pw = (QSnpImageWidget*)pWidget;
  if(!pw)
      return QERR_ERROR;
  if(!pw->userPointSet())
      return QERR_ERROR;

  QPoint pt = pw->userPoint();
  pt /= scaleFactors[""];

  point->x = pt.x();
  point->y = pt.y();

  return QERR_NO_ERROR;
}

// получение координат пользовательского прямоугольника
QError QSnpImageView::getUserRect(QSnp::SnpRect* rect)
{
  QSnpImageWidget* pw = (QSnpImageWidget*)pWidget;
  if(!pw)
      return QERR_ERROR;
  if(!pw->userRectSet())
      return QERR_ERROR;

  QRect rc = pw->userRect();
  double scale = scaleFactors[""];

  rect->x = rc.x()/scale;
  rect->y = rc.y()/scale;
  rect->width = rc.width()/scale;
  rect->height = rc.height()/scale;

  return QERR_NO_ERROR;
}

// функция удаления всех фигур в данном окне
QError  QSnpImageView::clearFigures()
{
  foreach(QSnpFigure* pf, lsFigures)
    delete pf;

  lsFigures.clear();
  QWidget* pWidget = getWidget();
  if(pWidget)
    pWidget->update();
  return QERR_NO_ERROR;
}

void QSnpImageView::showWidget(bool bShow)
{
  QSnpImageWidget* pw = (QSnpImageWidget*)pWidget;
  if(bShow) 
    scrollArea.show();
  else 
    scrollArea.hide();
}

void QSnpImageView::onWheelEvent(QWheelEvent* ev)
{
  QSnpImageWidget* pw = (QSnpImageWidget*)pWidget;
  if(!pw)
    return;
  pw->onWheelEvent(ev);
}

void QSnpImageView::onHorSliderMoved(int value)
{
  scrollArea.horizontalScrollBar()->setSliderPosition(value);
}

void QSnpImageView::onVerSliderMoved(int value)
{
  scrollArea.verticalScrollBar()->setSliderPosition(value);
}
