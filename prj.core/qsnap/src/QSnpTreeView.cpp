/**
  \file   QSnpTreeView.cpp
  \brief  Functions for QSnpTreeView class represents View to show QSnap control tree
  \author Sholomov D.
  \date   15.5.2013
*/

#include "QSnpTreeView.h"

using namespace QSnp;

QSnpTreeView::QSnpTreeView(void)
{
  pTreeWidget = NULL;
  eViewType = VT_SNAPTREE_VIEW;
}

QSnpTreeView::~QSnpTreeView(void)
{
  Destroy();
}

QHandle QSnpTreeView::Create(QSnpInstance* _pInstance)
{
  if(pTreeWidget)
    return (QHandle)this;

  // настройка окна дерева снепа
  pTreeWidget = new QSnpTreeWidget();
  QTreeWidget* pw = pTreeWidget;
  pw->setHeaderHidden(true);
  pw->setItemsExpandable(true);
  pw->setRootIsDecorated(true);

  QTreeWidgetItem* pri = new QTreeWidgetItem();
  pri->setText(0, getRootNode()->caption());
  pw->addTopLevelItem(pri);
  pTreeWidget->setQSnpNode(getRootNode(), pri, 0);

  rootNode.setTreeWidgetItem(pri);
  rootNode.setInstance(_pInstance);
 
  pri->setExpanded(true);

  // настройка окна свойств
  pPropWidget = new QSnpPropertyWidget();
  QStringList lst; lst << "Property" << "Value";
  pPropWidget->setHorizontalHeaderLabels(lst);

  pw->resize(500,300);
  pw->move(200,100);
  
  // настройка сплиттера
  splitter.setOrientation(Qt::Horizontal);
  splitter.addWidget(pTreeWidget);
  splitter.addWidget(pPropWidget);

  splitter.show();
  pTreeWidget->show();
  pPropWidget->show();

  pTreeWidget->connect(
    pTreeWidget, SIGNAL(itemSelectionChanged()),
    this, SLOT(onUpdateProperties())
    );

  pPropWidget->connect(
    pPropWidget, SIGNAL(cellChanged(int, int)),
    this, SLOT(onCellChanged(int, int))
    );

  QSnpNode* pNode = getRootNode();

  return (QHandle)this;

}

bool QSnpTreeView::Destroy(void)
{
  if(!pTreeWidget)
    return true;
  delete pTreeWidget;
  pTreeWidget = 0;
  return true;

}

QSnpRootNode* QSnpTreeView::getRootNode()
{
  return &rootNode;
}

/// добавление дочерней вершины
QSnpNode* QSnpTreeView::addNode(QSnpNode* pParentNode, NodeInfo& nodeInfo)
{
  pParentNode->addChildNode(nodeInfo);
  return NULL;
}

/// удаление вершины
QError QSnpTreeView::removeNode(QSnpNode* pNode)
{
  return NULL;
}

// обновление свойств текущей вершины
void QSnpTreeView::onUpdateProperties()
{
  QSnpNode* pNode = pTreeWidget->getCurrentQSnpNode();
  if(!pNode)
    return;
  pPropWidget->updateProperties((QHandle)pNode);
}

// изменение значения свойства
void QSnpTreeView::onCellChanged(int row, int column)
{
  if(column!=1)
    return;

  QSnpNode* pNode = pTreeWidget->getCurrentQSnpNode();
  if(!pNode)
    return;

  QString key = pPropWidget->item(row,0)->text();
  QString val = pPropWidget->item(row,1)->text();

  SetNodeValue(pNode->handle(), key.toStdString().c_str(), val.toStdString().c_str());
  SaveNodeProperty(pNode->handle(), key.toStdString().c_str());
}

// загрузить состояние вершин
void QSnpTreeView::LoadState()
{
  if(!pTreeWidget)
    return;
  rootNode.loadState(true);
}

// сохранить состояние вершин
void QSnpTreeView::SaveState()
{
  if(!pTreeWidget)
    return;
  rootNode.saveState(true);
}


