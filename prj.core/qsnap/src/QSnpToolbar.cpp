/**
  \file   QSnpToolBar.cpp
  \brief  Functions for QWidget based class to show toolbar
  \author Sholomov D.
  \date   17.3.2014
*/

#include "QSnpToolbar.h"
#include <QActionEvent>
#include <QAction>
#include "qsnap/qsnap.h"
#include "QSnpInstance.h"

using namespace QSnp;

QSnpToolBar::QSnpToolBar(void)
{
  setWindowFlags(Qt::Tool);
}

QSnpToolBar::~QSnpToolBar(void)
{
}

void QSnpToolBar::onAction()
{
  QAction *pAction = qobject_cast<QAction*>(sender());
  if(!pAction)
    return;
  int hEvent = pAction->data().toInt();
  if(hEvent)
  {
    QSnpEvent* pEvent = (QSnpEvent*)hEvent;
    FireEventId(
      (QHandle)pEvent->getInstance(), 
      pEvent->sId.toStdString().c_str(), 
      0, 0);
  }
  return;
}
