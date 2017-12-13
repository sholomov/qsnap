/**
  \file   QSnpTreeView.h
  \brief  QSnpTreeView class represents View to show QSnap control tree
  \author Sholomov D.
  \date   15.5.2013
*/

#pragma once

#include <qsnap/qsnap.h>

#include "QSnpView.h"
#include "QSnpNode.h"
#include "QSnpTreeWidget.h"
#include "QSnpPropertyWidget.h"

#include <QSplitter>

class QSnpNode;
class QSnpRootNode;

class QSnpTreeView : public QSnpView
{
  Q_OBJECT
public:
  QSnpTreeView(void);
  virtual ~QSnpTreeView(void);

  /// создание окна
  virtual QHandle Create(QSnpInstance* _pInstance);

  /// разрушение окна
  virtual bool Destroy();

  /// получение корневой вершины
  QSnpRootNode* getRootNode();
  
  /// добавление дочерней вершины
  QSnpNode* addNode(QSnpNode* pParentNode, QSnp::NodeInfo& nodeInfo);

  /// удаление вершины
  QError removeNode(QSnpNode* pNode);

  /// получение виджета
  virtual QWidget* getWidget() { return &splitter; }

  /// получение виджета
  virtual QSnpTreeWidget* getTreeWidget() { return pTreeWidget; }

  // загрузить состояние вершин
  void LoadState();

  // сохранить состояние вершин
  void SaveState();

public slots:
  /// обновление свойств текущей вершины
  void onUpdateProperties();

  /// изменение значения свойства
  void onCellChanged(int row, int column);


private:
  // Widgets
  QSnpTreeWidget* pTreeWidget;      // Qt widget для работы с деревом
  QSnpPropertyWidget* pPropWidget;  // Qt widget для свойств
  QSplitter splitter;

  QSnpRootNode rootNode;
  
};
