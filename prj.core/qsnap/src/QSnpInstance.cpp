/**
  \file   QSnpInstance.cpp
  \brief  Functions for QSnpInstance class represents QSnap Instance object
  \author Sholomov D.
  \date   15.5.2013
*/

#include "QSnpInstance.h"

QSnpInstance::QSnpInstance(void)
{
  pTreeView = NULL;
}

QSnpInstance::~QSnpInstance(void)
{
  // удаление всех незакрытых View
  for(QList<QSnpView*>::iterator it=lstViews.begin(); it!=lstViews.end(); it++)
  {
    if(*it!=NULL)
      delete *it;
  }

  return;
}

// добавление события
QHandle QSnpInstance::addEvent(const char* pId, const char* pParm, void* pParmEx)
{
  lstEvents.push_back(QSnpEvent((QHandle)this, pId, pParm, pParmEx));
  QSnpEvent& ev = lstEvents.back();
  return (QHandle)&ev;
}

// удаление события
bool QSnpInstance::removeEvent(QHandle hEvent)
{
  for(QList<QSnpEvent>::iterator it=lstEvents.begin(); it!=lstEvents.end(); it++)
  {
    QSnpEvent& ev = *it;
    if(ev.handle() == hEvent)
    {
      lstEvents.erase(it);
      return true;
    }
  }
  return false;
}

bool QSnpInstance::addView(QSnpView* pView)
{
  lstViews.push_back(pView);
  return true;
}

bool QSnpInstance::removeView(QSnpView* pView)
{
  for(QList<QSnpView*>::iterator it=lstViews.begin(); it!=lstViews.end(); it++)
  {
    if(*it!=NULL && *it==pView)
      *it=NULL;
  }
  return true;
}


// перерисовка всех view-окон инстанса
bool QSnpInstance::updateViews()
{
  for(QList<QSnpView*>::iterator it=lstViews.begin(); it!=lstViews.end(); it++)
  {
    QSnpView* pView = *it;
    if(!pView)
      continue;
    QWidget* pWidget = pView->getWidget();
    if(pView!=NULL && pWidget!=NULL)
      pWidget->update();
  }
  return true;
}

// получение хэндла события по id
QHandle QSnpInstance::getEvent(const char* pId)
{
  for(QList<QSnpEvent>::iterator it=lstEvents.begin(); it!=lstEvents.end(); it++)
  {
    QSnpEvent& ev = *it;
    if(ev.sId == pId)
      return (QHandle)&ev;
  }
  return 0;
}

