/**
  \file   eventfilters.cpp
  \brief  Function members for filtering events classes
  \author Sholomov D.
  \date   03.06.2013
*/
#include "eventfilters.h"
#include "QSnpInstance.h"

#include <QApplication>
#include <QMainWindow>
#include <QKeyEvent>

UserInputEventFilter::UserInputEventFilter(
    QSnp::WaitUserFlags   wf,
    QSnp::WaitOptions*    pwo, 
    QObject*              obj) :  QObject(obj), evn(QEvent::None), evnKey(QEvent::None, 0, 0)
{ 
  evn = QEvent(QEvent::None);
  nTimeInterval=0;
  waitFlags = wf;
  pWaitOptions = pwo;
  evnTimer = new QTimerEvent(0);
  nEventType = UIE_EVENT_NONE;
  pInstance = NULL;
}

UserInputEventFilter::~UserInputEventFilter()
{
  if(evnTimer)
  {
    delete evnTimer;
    evnTimer = NULL;
  }
}

bool UserInputEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    // завершение работы
    if(event->type() == QEvent::Close || event->type() == QEvent::Destroy || event->type() == QEvent::Quit)
    {
      // только при закрытии главного окна
      QMainWindow* pMainWnd = dynamic_cast<QMainWindow*>(obj);
      if(pMainWnd)
      {
        evn = *event;
        qApp->exit(0);
        return true;
      }
    }

    // события клавиатуры
    if (  event->type() == QEvent::KeyPress/* || event->type() == QEvent::KeyRelease*/ )
    {
      QKeyEvent* pEvent = (QKeyEvent*)event;
      quint32 virtKey = pEvent->nativeVirtualKey();

      // не реагировать на клавиши-модификаторы Shift-Ctrl-Alt
      if(virtKey==WF_VKEY_SHIFT || virtKey==WF_VKEY_CTRL || virtKey==WF_VKEY_ALT )
        return QObject::eventFilter(obj, event);        // standard event processing

      // если установлен фильтр - не по клавиатуре
      if(!(waitFlags & WF_KEYBOARD))
        return QObject::eventFilter(obj, event);        // standard event processing
      
      // если установлен фильтр для определенной кнопке, но нажата не она
      if( (waitFlags & WF_KEYBOARD) && 
          pWaitOptions &&
          pWaitOptions->nKbdEventFilter == WKF_KEY &&
          pWaitOptions->nKbdFilterData != pEvent->nativeVirtualKey()
          )
        return QObject::eventFilter(obj, event);        // standard event processing

      evn = *event;
      evnKey = *pEvent;
      return false;
    }
    // события мыши
    else if ( event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonDblClick)
    {
      // если установлен фильтр - не по мыши
      if(!(waitFlags & WF_MOUSE))
        return QObject::eventFilter(obj, event);        // standard event processing
      
      evn = *event;
      return false;
    }
    // события таймера
    else if (event->type() == QEvent::Timer && (waitFlags & WF_TIMER))
    {
      QTime timeCurr = QTime::currentTime();
      QTime timeElapse = QTime(timeSet).addMSecs(nTimeInterval);

      if(timeCurr >= timeElapse)
      {
        evn = *event;
      }
      return false;
    }
    // события пользователя
    else if (event->type() == QEvent::Timer && (waitFlags & WF_USER))
    {
      for(QList<QSnpEvent>::iterator it = pInstance->lstFiredEvents.begin(); 
        it!=pInstance->lstFiredEvents.end(); 
        it++) 
      {
        QSnpEvent& ev = *it;

        if(!pWaitOptions->sUserEventIds[0]) // нет фильтра по событиям
        {
          // запомним произошедшее событие
          sFiredEventId = pWaitOptions->sUserEventIds[0];
          //удалим это событие из списка произошедших
          pInstance->lstFiredEvents.erase(it);
          evn = *event;
          return false;
        }

        for(int nId=0; nId<QS_MAX_EVENTS; nId++)
        {
          const char* pId = pWaitOptions->sUserEventIds[nId];
          if(pId && ev.sId==pId)
          {
            // запомним произошедшее событие
            sFiredEventId = pId;
            //удалим это событие из списка произошедших
            pInstance->lstFiredEvents.erase(it);
            evn = *event;
            return false;
          }
        }
      }
    }
    else return QObject::eventFilter(obj, event);        // standard event processing
    
    return false;
}

void UserInputEventFilter::onTime()
{
  if(waitFlags & WF_TIMER)
    nEventType = UIE_EVENT_TIMER;
}

bool UserInputEventFilter::eventSignaled()
{ 
  return evn.type() != QEvent::None; 
}

QEvent* UserInputEventFilter::getEvent()
{ 
  if(nEventType == UIE_EVENT_TIMER)
    return evnTimer;
  else 
    return &evn; 
}

QKeyEvent* UserInputEventFilter::getKeyEvent()
{ 
  return &evnKey; 
}

QTime UserInputEventFilter::setTimeInterval(int msec) 
{ 
  timeSet = QTime::currentTime();
  nTimeInterval = msec;
  return QTime(timeSet).addMSecs(msec); 
}

void UserInputEventFilter::setInstance(QSnpInstance* _pInstance)
{
  pInstance=_pInstance;
}
