/**
  \file   QSnpFigure.cpp
  \brief  Class function members for polymorph classes for drawing figures in QImageView
  \author Sholomov D.
  \date   15.5.2013
*/

#include "QSnpFigure.h"
#include "QSnpView.h"

/////////////////////////////////////////////////////////////////
////  QSnpFigure

QSnpFigure::QSnpFigure(QSnpView* p) 
{
  bVisible = true;
  pView = p;
}

QSnpFigure::~QSnpFigure(void)
{
}

void QSnpFigure::setVisible(bool _bVisible)
{  
  bVisible = _bVisible;
  QSnpView* pView = getView();
  if(!pView)
    return;
  QWidget* pWidget = pView->getWidget();
  if(!pWidget)
    return;
  pWidget->repaint();
}

/////////////////////////////////////////////////////////////////
////  QSnpLine

QSnpLine::QSnpLine(QSnpView* pView) : QSnpFigure(pView)
{
  color = Qt::red;
  lineWidth = 1;
}

QSnpLine::~QSnpLine(void)
{
}

void QSnpLine::Draw(QPainter* pPainter, float ratio)
{
  pPainter->setRenderHint(QPainter::Antialiasing, true);
  pPainter->setPen( QPen(color, lineWidth) );
  
  QPoint ptImageFrom = QPoint_scale(ptFrom, ratio);
  QPoint ptImageTo = QPoint_scale(ptTo, ratio);

  pPainter->drawLine(ptImageFrom, ptImageTo);
}

/////////////////////////////////////////////////////////////////
////  QSnpRect

QSnpRect::QSnpRect(QSnpView* pView) : QSnpFigure(pView)
{
  color = Qt::red;
  lineWidth = 1;
}

QSnpRect::~QSnpRect(void)
{
}

void QSnpRect::Draw(QPainter* pPainter, float ratio)
{
  pPainter->setPen(QPen(color, lineWidth));
  pPainter->setBrush(QBrush(Qt::NoBrush));

  QRect rcImage = QRect_scale(rc, ratio);
  pPainter->drawRect(rcImage);
}

/////////////////////////////////////////////////////////////////
////  QSnpEllipse

QSnpEllipse::QSnpEllipse(QSnpView* pView) : QSnpFigure(pView)
{
  color = Qt::red;
  lineWidth = 1;
}

QSnpEllipse::~QSnpEllipse(void)
{
}

void QSnpEllipse::Draw(QPainter* pPainter, float ratio)
{
  pPainter->setPen(QPen(color, lineWidth));
  pPainter->setBrush(QBrush(Qt::NoBrush));

  QRect rcImage = QRect_scale(rc, ratio);
  pPainter->drawEllipse(rcImage);
}

/////////////////////////////////////////////////////////////////
////  QSnpText

QSnpText::QSnpText(QSnpView* pView) : QSnpFigure(pView)
{
  color     = Qt::red;
  lineWidth = 2;
  alignment = (int)(Qt::AlignLeft | Qt::AlignTop);
  bBold     = false;
  bItalic   = false;
  fontType  = "Consolas";
  textValue = "";
}

QSnpText::~QSnpText(void)
{
}

void QSnpText::Draw(QPainter* pPainter, float ratio)
{
  pPainter->setPen(QPen(color, lineWidth));
  pPainter->setBrush(QBrush(Qt::NoBrush));
  
  QFont font(fontType, fontSize);
  font.setBold(bBold);
  font.setItalic(bItalic);
  pPainter->setFont(font);

  QRect rcImage = QRect_scale(rc, ratio);
  pPainter->drawText(rcImage, textValue, QTextOption((Qt::AlignmentFlag)alignment));
}

/////////////////////////////////////////////////////////////////
////  QSnpPoint

QSnpPoint::QSnpPoint(QSnpView* pView, int x, int y, int width) : QSnpFigure(pView)
{
  color = Qt::red;
  lineWidth = width;
  rc = QRect(x - lineWidth/2, y - lineWidth/2, lineWidth, lineWidth);
  mX = x;
  mY = y;

}

QSnpPoint::~QSnpPoint(void)
{
}

void QSnpPoint::Draw(QPainter* pPainter, float ratio)
{
  pPainter->setRenderHint(QPainter::Antialiasing, true);
  pPainter->setPen(QPen(color, 1));
  pPainter->setBrush(QBrush(color));

  QRect rcImage(
    float_scale(mX+0.5,ratio)-lineWidth/2, 
    float_scale(mY+0.5,ratio)-lineWidth/2,
    lineWidth, 
    lineWidth
    );

  pPainter->drawEllipse(rcImage);
}