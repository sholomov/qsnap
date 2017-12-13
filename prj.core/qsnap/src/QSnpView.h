/**
  \file   QSnpView.h
  \brief  QSnpView class is a basic class for all QSnap View classes
  \author Sholomov D.
  \date   15.5.2013
*/

#pragma once

#include <qsnap/qsnap.h>
#include <QString>
#include <QWidget>

class QSnpInstance;

class QSnpView : public QObject
{
public:
  QSnpView(void);
  virtual ~QSnpView(void);

  /// создание окна
  virtual QHandle Create(QSnpInstance* _pInstance);

  /// разрушение окна
  virtual bool Destroy() { return true; }

  QSnpInstance* getInstance() { return pInstance; }
  void setInstance(QSnpInstance* _pInstance) { pInstance=_pInstance; }
  
  /// тип окна
  QSnp::ViewType getViewType() { return eViewType; }

  /// хэндл дерева
  QHandle getViewHandle() { return (QHandle)this; }

  /// идентификатор окна
  QString getId() { return sId; }
  void setId(const char* id) { sId = id; }

  /// заголовок окна
  QString getCaption() { return sCaption; }
  void setCaption(const char* caption) { sCaption = caption; }

  virtual QWidget* getWidget();
  virtual QWidget* getFrameWidget();

  const char* getWindowTitleBuff() { return sWindowTitleBuff.c_str(); }
  void setWindowTitleBuff(const char* title) { sWindowTitleBuff = title; }

  const char* getDefaultConfig() { return sConfigDefault.c_str(); }
  void setDefaultConfig(const char* config) { sConfigDefault = config; }

  /// полиморфная функция показа окна
  virtual void showWidget(bool bShow);

  /// чтение свойств окна
  virtual bool loadProperties(const char* pConfig = 0);

  /// сохранение свойств окна
  virtual bool saveProperties(const char* pConfig = 0);

protected:
  QString sId;
  QString sCaption;

  QSnpInstance* pInstance;        // ссылка на инстенс Snap-а
  QSnp::ViewType eViewType;

  QWidget* pWidget;               // Qt widget для работы с деревом
  std::string sWindowTitleBuff;   // название окна
  std::string sConfigDefault;

};

void LoadWindowPos(QWidget* pWidget, const char* pConfigName);
void SaveWindowPos(QWidget* pWidget, const char* pConfigName);