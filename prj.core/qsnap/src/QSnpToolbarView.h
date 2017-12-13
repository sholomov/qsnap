/**
  \file   QSnpToolbarView.h
  \brief  QSnpToolbarView class represents View to toolbar control panel
  \author Sholomov D.
  \date   14.3.2014
*/

#pragma once
#include <qsnap/qsnap.h>

#include "QSnpView.h"

class QSnpToolbarView :
  public QSnpView
{
public:
  QSnpToolbarView(void);
  virtual ~QSnpToolbarView(void);

  /// создание окна
  virtual QHandle Create(QSnpInstance* pInstance);

  /// разрушение окна
  virtual bool Destroy();

};
