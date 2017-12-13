/**
  \file   QSnpImageWidget.cpp
  \brief  Functions for QWidget based class to show and interact with image
  \author Sholomov D.
  \date   15.5.2013
*/

#include "QSnpImageWidget.h"
#include "QSnpImageView.h"

#include <QApplication>
#include <QScrollBar>
#include <QKeyEvent>
#include <QEvent>

//////////////////////////////////////////////////////////////////////////////
//// Класс StwEventFilter Фильтр событий для перехвата событий аппликации

class SiwEventFilter : public QObject
{
public:
  SiwEventFilter(QObject* obj = 0) : QObject(obj) { pWidget=static_cast<QSnpImageWidget*>(obj); }

protected:
  // основная функция фильтрации событий
  virtual bool eventFilter(QObject *obj, QEvent *event)
  {
    if ( event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease )
    {
      QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
      if(pWidget)
        pWidget->kbdModifiers = keyEvent->modifiers();
      return false;
    }
    else return QObject::eventFilter(obj, event);        // standard event processing
 }

private:
  class QSnpImageWidget* pWidget;  // класс, реагирующий на события аппликации

};

//////////////////////////////////////////////////////////////////////////////
//// Класс QSnpImageWidget 

QSnpImageWidget::QSnpImageWidget(QSnpImageView* pView)
{
  bUserPointSet = false;
  bUserRectSet = false;
  bMouseIsDragging = false;
  bPictureDragging = false;
  ratio = 1.0;
  qApp->installEventFilter(new SiwEventFilter(this));
  pImageView = pView;
  pParentImageView = NULL;
}

QSnpImageWidget::~QSnpImageWidget(void)
{
}

const int radusPt = 2;

void	QSnpImageWidget::paintEvent(QPaintEvent* ev)
{
  QPainter* pPainter = &painter;

  painter.begin(this);

  // отрисовка изображения
  painter.drawImage(rect(), image, image.rect());

  // отрисовка фигур
  foreach (QSnpFigure* pf, *plsFigures)
  {
    if(pf->bVisible)
      pf->Draw(&painter, ratio);
  }

  // отрисовка пользовательской точки
  if(bUserPointSet)
  {
    int diamPt = radusPt*2+1;
    pPainter->setPen(QPen(Qt::red, 3));
    pPainter->setBrush(QBrush(Qt::red));
    
    // центрирование по центру точки изображениято
    QRect rcUserPt = QRect(
      float_scale(float(ptUserPoint.x())+0.5, ratio)-radusPt, 
      float_scale(float(ptUserPoint.y())+0.5, ratio)-radusPt, 
      diamPt, 
      diamPt
      );
    
    pPainter->drawEllipse(rcUserPt);
  }
  
  // отрисовка пользовательского прямоугольника
  if(bUserRectSet || bMouseIsDragging)
  {
    int diamPt = radusPt*2+1;
    pPainter->setPen(QPen(Qt::yellow, 3));
    pPainter->setBrush(Qt::NoBrush);

    QRect rcImage = QRect_scale(rcUserRect, ratio);
    pPainter->drawRect(rcImage);
  }
  
  painter.end();

  // сохранение свойств окошка
  getView()->saveProperties();

  // сохранение свойств окошка для SyncView
  if(getParentView())
    getParentView()->saveProperties();

}

const int lineWd=3;

void	QSnpImageWidget::mousePressEvent(QMouseEvent* ev)
{
  setCursor(cursor);

  // обработка выставления точки (при нажатии CTRL)
  if( kbdModifiers & Qt::ControlModifier &&
      ev->button() & Qt::LeftButton
      )
  {
    int diamPt = (radusPt+1)*2+1;

    QRect rcOldUserPt(ptUserPoint.x()-(radusPt+1), ptUserPoint.y()-(radusPt+1), diamPt, diamPt);

    ptUserPoint = QPoint(float(ev->pos().x())/ratio, float(ev->pos().y())/ratio);
    bUserPointSet = true;

    QRect rcUserPt(ptUserPoint.x()-(radusPt+1), ptUserPoint.y()-(radusPt+1), diamPt, diamPt);

    repaint(); // можно оптимизировать
    //repaint(QRect_scale(rcUserPt, ratio));
    //repaint(QRect_scale(rcOldUserPt, ratio));

  }

  // обработка начала выставления прямоугольника(при нажатии SHIFT)
  if( (kbdModifiers & Qt::ShiftModifier) &&
      (ev->button() & Qt::LeftButton)
      )
  {
    bMouseIsDragging = true;
    ptFirstDragPoint = QPoint_scale(ev->pos(), float(1)/ratio);
  }

  // обработка начала движения картинки
  if( kbdModifiers==0 &&
      (ev->button() & Qt::LeftButton &&
      getView()->getViewType() == QSnp::VT_IMAGE_VIEW) // только для обычных ImageView
      )
  {
    bPictureDragging = true;
    ptFirstDragPoint = QPoint_scale(ev->pos(), 1.0);

    int horSliderPos = getView()->getScrollArea()->horizontalScrollBar()->sliderPosition();
    int verSliderPos = getView()->getScrollArea()->verticalScrollBar()->sliderPosition();
    ptFirstSliderPos = QPoint(horSliderPos, verSliderPos);

    cursor.setShape(Qt::ClosedHandCursor);
    qApp->setOverrideCursor(cursor);
  }

}

void	QSnpImageWidget::mouseMoveEvent(QMouseEvent* ev)
{
  // обработка выставления прямоугольника(при нажатии SHIFT)
  if(bMouseIsDragging)
  {
    QRect rcOldUserRect = rcUserRect;

    ptSecondDragPoint = QPoint_scale(ev->pos(), float(1)/ratio);

    rcUserRect = QRect(
      qMin(ptFirstDragPoint.x(),ptSecondDragPoint.x()),
      qMin(ptFirstDragPoint.y(),ptSecondDragPoint.y()),
      qAbs(ptSecondDragPoint.x()-ptFirstDragPoint.x()),
      qAbs(ptSecondDragPoint.y()-ptFirstDragPoint.y())
      );
   QRect rcPaint(
      qMin(ptFirstDragPoint.x(),ptSecondDragPoint.x())-lineWd,
      qMin(ptFirstDragPoint.y(),ptSecondDragPoint.y())-lineWd,
      qAbs(ptSecondDragPoint.x()-ptFirstDragPoint.x())+lineWd*2,
      qAbs(ptSecondDragPoint.y()-ptFirstDragPoint.y())+lineWd*2
      );
    rcPaint.united(rcOldUserRect);

    repaint(); // оптимизировать
  }
  // обработка перемещения картинки
  if(bPictureDragging)
  {
    ptSecondDragPoint = QPoint_scale(ev->pos(), 1.0);

    ptFirstSliderPos = QPoint(0,0);

    int horSliderPos = getView()->getScrollArea()->horizontalScrollBar()->sliderPosition();
    int verSliderPos = getView()->getScrollArea()->verticalScrollBar()->sliderPosition();
    
    horSliderPos -= ptSecondDragPoint.x() - ptFirstDragPoint.x();
    verSliderPos -= ptSecondDragPoint.y() - ptFirstDragPoint.y();

    onHorSliderMoved(horSliderPos);
    onVerSliderMoved(verSliderPos);

    //ptFirstDragPoint = ptSecondDragPoint;

    //repaint(); // оптимизировать
  }
  if(!bMouseIsDragging && !bPictureDragging)
  {
    bMouseIsDragging = false;
    ptFirstDragPoint = QPoint(0,0);
    ptSecondDragPoint = QPoint(0,0);
  }
}

void	QSnpImageWidget::mouseReleaseEvent(QMouseEvent* ev)
{
  // выставление пользовательского прямоугольника
  if(bMouseIsDragging)
  {
    bUserRectSet = true;
    bMouseIsDragging = false;
    repaint();
    ptFirstDragPoint = QPoint(0,0);
    ptSecondDragPoint = QPoint(0,0);
    ptFirstSliderPos = QPoint(0,0);
  }
  if(bPictureDragging)
  {
    bPictureDragging = false;
    repaint();
    ptFirstDragPoint = QPoint(0,0);
    ptSecondDragPoint = QPoint(0,0);
    ptFirstSliderPos = QPoint(0,0);
  }
  cursor.setShape(Qt::ArrowCursor);
  qApp->restoreOverrideCursor();

}

void QSnpImageWidget::wheelEvent(QWheelEvent* ev)
{
  getView()->onWheelEvent(ev);
}

void QSnpImageWidget::onWheelEvent(QWheelEvent* ev)
{
  if(!pScrollArea)
    return;

//  int numDegrees = ev->delta() / 8;
//  int numSteps = numDegrees / 15;

  int xPos = pScrollArea->horizontalScrollBar()->sliderPosition();
  int yPos = pScrollArea->verticalScrollBar()->sliderPosition();

  int xMaxPos1 = pScrollArea->horizontalScrollBar()->maximum();
  int yMaxPos1 = pScrollArea->verticalScrollBar()->maximum();

  //scrollArea
  ratio = ev->delta()<0 ? ratio*1.1 : ratio/1.1;
  resize(ratio*image.width(), ratio*image.height());

  int xMaxPos2 = pScrollArea->horizontalScrollBar()->maximum();
  int yMaxPos2 = pScrollArea->verticalScrollBar()->maximum();

  pScrollArea->horizontalScrollBar()->setSliderPosition((float)xPos/xMaxPos1*xMaxPos2);
  pScrollArea->verticalScrollBar()->setSliderPosition((float)yPos/yMaxPos1*yMaxPos2);

  repaint();
}

void QSnpImageWidget::onHorSliderMoved ( int value )
{
  getView()->onHorSliderMoved(value);
}

void QSnpImageWidget::onVerSliderMoved ( int value )
{
  getView()->onVerSliderMoved(value);
}