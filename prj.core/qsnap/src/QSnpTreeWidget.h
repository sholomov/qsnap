/**
  \file   QSnpTreeWidget.h
  \brief  QWidget based class to show and interact with QSnap control Tree
  \author Sholomov D.
  \date   15.5.2013
*/

#pragma once

#include "QSnpNode.h"

class QSnpTreeWidget : public QTreeWidget
{
  Q_OBJECT
public:
  QSnpTreeWidget(void);
  ~QSnpTreeWidget(void);

  QSnpNode* getQSnpNode(QTreeWidgetItem * item, int column=0);
  QSnpNode* getCurrentQSnpNode();

  void setQSnpNode(QSnpNode* pNode, QTreeWidgetItem * item, int column=0);
  void selectItem(QTreeWidgetItem * item, bool bSelect, bool bChildren = false);
  void expandItem(QTreeWidgetItem * item, bool bSelect, bool bChildren = false);

public slots:
  void onItemDoubleClicked(QTreeWidgetItem * item, int column );
  
protected: // events

public:
  Qt::KeyboardModifiers kbdModifiers; // последние модификаторы нажатия клавиши
};
