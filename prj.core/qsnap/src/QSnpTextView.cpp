/**
  \file   QSnpTextView.cpp
  \brief  Functions for QSnpTextView class represents View to show text Log
  \author Sholomov D.
  \date   15.5.2013
*/

#include "QSnpTextView.h"
#include "QSnpTextEdit.h"

#include <QScrollBar>

using namespace QSnp;

QSnpTextView::QSnpTextView(void)
{
  pWidget = NULL;
  eViewType = VT_TEXT_VIEW;
}

QSnpTextView::~QSnpTextView(void)
{
  Destroy();
}

QHandle QSnpTextView::Create(QSnpInstance* _pInstance)
{
  if(pWidget)
    return (QHandle)this;
  
  QSnpTextEdit* pWidget = new QSnpTextEdit();
  pWidget->resize(500,300);
  pWidget->show();
  pWidget->setPlainText("");
  pWidget->setReadOnly(true);
  pWidget->setFontFamily("Courier");

  QSnpView::pWidget = pWidget;

  return (QHandle)this;
}

bool QSnpTextView::Destroy(void)
{
  if(!pWidget)
    return true;
  delete pWidget;
  pWidget = 0;
  return true;
}

void QSnpTextView::Log(const char* str)
{
  if(!pWidget)
    return;
  QSnpTextEdit* pWidget = (QSnpTextEdit*)QSnpView::pWidget;
  pWidget->insertPlainText(str);
  
  // скроллировать вниз 
  QScrollBar* pVerScroll = pWidget->verticalScrollBar();
  pVerScroll->setSliderPosition(pVerScroll->maximum());
}

void QSnpTextView::Clear()
{
  if(!pWidget)
    return;
  QSnpTextEdit* pWidget = (QSnpTextEdit*)QSnpView::pWidget;
  pWidget->clear();
}
  

