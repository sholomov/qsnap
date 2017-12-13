/**
  \file   QSnpInstance.h
  \brief  QSnpInstance class represents QSnap Instance object
  \author Sholomov D.
  \date   15.5.2013
*/

#pragma once

#include "QSnpTreeView.h"
#include <QList>

class QSnpEvent;

class QSnpInstance
{
public:
  // конструктор/деструктор
  QSnpInstance(void);
  virtual ~QSnpInstance(void);

  // получение хэндля инстанса
  QHandle handle() { return QHandle(this); }

  // получение/установка дерева снепа, единственное для инстанса
  QSnpTreeView* getTreeView() { return pTreeView; }
  void setTreeView(QSnpTreeView* pView) { pTreeView=pView; }

  // добавление/удаление view в список view-окон инстанса
  bool addView(QSnpView* pView);
  bool removeView(QSnpView* pView);

  // перерисовка всех view-окон инстанса
  bool updateViews();

  // добавление/удаление событий
  QHandle addEvent(const char* pId, const char* pParm=0, void* _pParmEx=0);
  bool removeEvent(QHandle hEvent);

  // получение хэндла события по id
  QHandle getEvent(const char* pId);

  // список произошедших событий
  QList<QSnpEvent> lstFiredEvents;

protected:
  QSnpTreeView* pTreeView;
  QList<QSnpEvent> lstEvents; // список пользовательских событий

  QList<QSnpView*> lstViews;  // список указателей на окна

};

// класс-оболочка для пользовательского события
class QSnpEvent
{
public:
  QSnpEvent(QHandle _hInstance=0, const char* pId=0, const char* pParm=0, void* _pParmEx=0) :
      sId(pId), sParm(pParm), pParmEx(_pParmEx), hInstance(_hInstance) { ; }
  virtual ~QSnpEvent() {}

  // хэндл события
  QHandle   handle() { return QHandle(this); }

  // хэндл инстанса
  QSnpInstance* getInstance() { return (QSnpInstance*)hInstance; }

  QString   sId;          ///< идентификатор
  QString   sParm;        ///< основной параметр
  void*     pParmEx;      ///< дополнительный параметр

protected:
  QHandle   hInstance;    ///< хэндл инстанса

};