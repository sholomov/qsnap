/**
  \file   QSnpToolbarView.cpp
  \brief  Functions for QWidget represents View to toolbar control panel
  \brief  Functions for QWidget based class to show and interact with image
  \author Sholomov D.
  \date   15.5.2013
*/

#include "QSnpToolbarView.h"
#include "QSnpToolbar.h"

using namespace QSnp;

QSnpToolbarView::QSnpToolbarView(void)
{
  pWidget = NULL;
  eViewType = VT_TOOLBAR;
}

QSnpToolbarView::~QSnpToolbarView(void)
{
  Destroy();
}

QHandle QSnpToolbarView::Create(QSnpInstance* _pInstance)
{
  QSnpView::Create(_pInstance);

  if(pWidget)
    return (QHandle)this;
  
  QToolBar* pWidget = new QSnpToolBar();
  pWidget->resize(500,300);
  pWidget->show();

/*
  pWidget->setPlainText("");
  pWidget->setReadOnly(true);
  pWidget->setFontFamily("Courier");
*/
  QSnpView::pWidget = pWidget;
  setInstance(_pInstance);

  return (QHandle)this;
}

bool QSnpToolbarView::Destroy(void)
{
  if(!pWidget)
    return true;
  delete pWidget;
  pWidget = 0;
  return true;
}
