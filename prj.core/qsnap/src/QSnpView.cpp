/**
  \file   QSnpView.cpp
  \brief  Functions for QSnpView class is a basic class for all QSnap View classes
  \author Sholomov D.
  \date   15.5.2013
*/

#include "QSnpView.h"
#include "QSnpInstance.h"
#include <QSettings>

using namespace QSnp;


QSnpView::QSnpView(void)
{
  eViewType = VT_UNDEFINED;
  pWidget = NULL;
  pInstance = NULL;
}

QSnpView::~QSnpView(void)
{
}

QHandle QSnpView::Create(QSnpInstance* _pInstance)
{ 
  pInstance = _pInstance;
  pInstance->addView(this);
  return QHANDLE_INVALID; 
}


QWidget* QSnpView::getWidget()
{ 
  return pWidget;
}

QWidget* QSnpView::getFrameWidget()
{ 
  return getWidget();
}

void QSnpView::showWidget(bool bShow)
{ 
  if(bShow)
    getWidget()->show();
  else 
    getWidget()->hide();
}

void LoadWindowPos(QWidget* pWidget, const char* pConfigName)
{
  QSettings sett("QSnap");
  sett.beginGroup(pConfigName);
  QPoint pos(
    sett.value(QString("/")+=QString(pConfigName)+="PosX").toInt(),
    sett.value(QString("/")+=QString(pConfigName)+="PosY").toInt()
  );
  pWidget->move(pos);

#define def_width   500
#define def_height  300

  QSize size(
    sett.value(QString("/")+=QString(pConfigName)+="Width").toInt(),
    sett.value(QString("/")+=QString(pConfigName)+="Height").toInt()
  );
  if(size.width()==0)
    size.setWidth(def_width);
  if(size.height()==0)
    size.setHeight(def_height);

  pWidget->resize(size);

  sett.endGroup();
}

void SaveWindowPos(QWidget* pWidget, const char* pConfigName)
{
  if(!pWidget)
    return;
  QSettings sett("QSnap");
  sett.beginGroup(pConfigName);

  sett.setValue(QString("/")+=QString(pConfigName)+="PosX", pWidget->pos().x());
  sett.setValue(QString("/")+=QString(pConfigName)+="PosY", pWidget->pos().y());

  sett.setValue(QString("/")+=QString(pConfigName)+="Width", pWidget->size().width());
  sett.setValue(QString("/")+=QString(pConfigName)+="Height", pWidget->size().height());

  sett.endGroup();
}

bool QSnpView::loadProperties(const char* _pConfig)
{
  const char* pConfig = _pConfig ? _pConfig : sConfigDefault.c_str();
  LoadWindowPos(getWidget(), pConfig);
  return true;
}

bool QSnpView::saveProperties(const char* _pConfig)
{
  const char* pConfig = _pConfig ? _pConfig : sConfigDefault.c_str();
  SaveWindowPos(getWidget(), pConfig);
  return true;
}