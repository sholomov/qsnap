/**
  \file   QSnpListWidget.h
  \brief  Functions for QListWidget based class to show list
  \author Sholomov D.
  \date   30.08.2016
*/

#pragma once

#include <QListWidget>

class QSnpListWidget  : public QListWidget 
{
  Q_OBJECT
public:
  QSnpListWidget(void);
  ~QSnpListWidget(void);
};
