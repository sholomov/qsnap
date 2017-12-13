/**
  \file   QSnpPropertyWidget.h
  \brief  QWidget based class to show Properties of a TreeView Node
  \author Sholomov D.
  \date   27.8.2013
*/

#include "QSnpPropertyWidget.h"

#include <QHeaderView>

using namespace QSnp;

QSnpPropertyWidget::QSnpPropertyWidget(void)
{
  setColumnCount(2);
  resizeRowsToContents();
  verticalHeader()->hide();
}

// обновление свойств текущей вершины
void QSnpPropertyWidget::updateProperties(QHandle hNode)
{
  if(hNode == QHANDLE_NULL)
    return;

  int nNodeCount = GetNodePropertiesCount(hNode);
  setRowCount(nNodeCount);

  NodePropertyInfo propInfo={0};
  for(int nNode = 0; nNode<nNodeCount; nNode++)
  {
    GetNodeProperty(hNode, nNode, &propInfo);

    // название свойства
    QTableWidgetItem* piKey = new QTableWidgetItem(QString(propInfo.sKey));
    piKey->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    setItem(nNode, 0, piKey);

    // значение свойства
    QTableWidgetItem* piValue = new QTableWidgetItem(QString(propInfo.sValue));
    setItem(nNode, 1, piValue);
  }
  resizeRowsToContents();
}
