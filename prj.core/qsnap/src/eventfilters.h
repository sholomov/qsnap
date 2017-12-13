/**
  \file   eventfilters.h
  \brief  Class for filtering events
  \author Sholomov D.
  \date   03.06.2013
*/

#ifndef EVENTFILTERS_H
#define EVENTFILTERS_H

#include <QObject>
#include <QTime>
#include <QKeyEvent>
#include "qsnap/qsnap_types.h"

class QSnpInstance;

//////////////////////////////////////////////////////////////////////////////
//// Класс StwEventFilter Фильтр событий для перехвата событий аппликации

class UserInputEventFilter : public QObject
{
  Q_OBJECT
public:
  UserInputEventFilter(
    QSnp::WaitUserFlags   waitFlags,
    QSnp::WaitOptions*    pwf, 
    QObject*              obj = 0
    );

  virtual ~UserInputEventFilter();

protected:
  // основная функция фильтрации событий
  virtual bool eventFilter(QObject *obj, QEvent *event);

public slots:
  void onTime();

public:
  bool    eventSignaled();
  QEvent* getEvent();
  QKeyEvent* getKeyEvent();

  QTime   setTimeInterval(int msec);
  void    setInstance(QSnpInstance* pInstance);

  QString getFiredEvent() { return sFiredEventId.toStdString().c_str(); }
  void    setFiredEvent(const char* s) { sFiredEventId=s; }

private:

#define UIE_EVENT_NONE      0
#define UIE_EVENT_TIMER     1
  int     nEventType;

  QEvent        evn;

  QKeyEvent     evnKey;
  QTimerEvent*  evnTimer;

  QTime   timeSet;
  int     nTimeInterval;

  QSnp::WaitUserFlags   waitFlags;
  QSnp::WaitOptions*    pWaitOptions;

  QSnpInstance*         pInstance;
  QString               sFiredEventId;
};

#endif // EVENTFILTERS_H
