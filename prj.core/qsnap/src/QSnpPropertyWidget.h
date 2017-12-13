/**
  \file   QSnpPropertyWidget.h
  \brief  QWidget based class to show Properties of a TreeView Node
  \author Sholomov D.
  \date   27.8.2013
*/

#pragma once

#include <qsnap/qsnap.h>

#include <QTableWidget>

class QSnpPropertyWidget : public QTableWidget
{
  Q_OBJECT

public:
  // конструктор / дестуктор
  QSnpPropertyWidget(void);
  virtual ~QSnpPropertyWidget(void) {}

  // обновление свойств текущей вершины
  void updateProperties(QHandle hNode);

};
