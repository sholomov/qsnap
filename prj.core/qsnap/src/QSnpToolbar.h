/**
  \file   QSnpToolbar.h
  \brief  QWidget based class to show toolbar
  \author Sholomov D.
  \date   17.3.2014
*/

#pragma once

#include <QToolBar>
#include <QAction>

class QSnpToolBar : public QToolBar
{
  Q_OBJECT
public:
  QSnpToolBar(void);
  ~QSnpToolBar(void);

public slots:
  void onAction();

};
