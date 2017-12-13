/**
  \file   QSnpNode.cpp
  \brief  Functions for QSnpNode class represents Node object in QSnpTreeView 
  \author Sholomov D.
  \date   15.5.2013
*/

#include "QSnpNode.h"
#include "QSnpInstance.h"

#include <QSettings>

using namespace QSnp;


QSnpNode::QSnpNode(const char* pId, const char* pCaption)
{
  sId = "";
  sCaption = "";
  if(pId)
    sId=pId;
  if(pCaption)
    sCaption=pCaption;
  pWidgetItem = new QTreeWidgetItem;
  pInstance = NULL;
  bSelected = false;
  bExpanded = false;
}

QSnpNode::~QSnpNode()
{
  /*
  foreach(QSnpNode pl, lstChildNode)
    pl.~QSnpNode();
  lstChildNode.clear();

  if(pWidgetItem)
  {
    pWidgetItem->parent()->removeChild(pWidgetItem);
    //delete pWidgetItem;
    pWidgetItem = NULL;
  }
  */
}

QSnpNode* QSnpNode::addChildNode(NodeInfo& nodeInfo)
{
  // создание новой вершины и добавление в список дочерних
  QSnpNode node(nodeInfo.szId, nodeInfo.szDescription);
  lstChildNode.push_back(node);
  QSnpNode* pNode = &lstChildNode.back();
  pNode->setInstance(pInstance);

  // сохранение в вершине ссыки на GUI-вершину 
  QTreeWidgetItem* pcwi = pNode->pWidgetItem;
  pcwi->setText(0,nodeInfo.szDescription);

  // сохранение в GUI-вершине ссылки на вершинв QSnpNode 
  QVariant qData((QHandle)pNode);
  pcwi->setData(0, TID_QNODE_REF, qData);
  pWidgetItem->addChild(pcwi);

  return &lstChildNode.back();
}

// поиск вершины с данным id (первой попавшейся)
QSnpNode* QSnpNode::findNode(const char* pId)
{
  if(sId == pId)
    return this;

  for(QList<QSnpNode>::iterator i = lstChildNode.begin(); i!=lstChildNode.end(); i++)
  {
    QSnpNode* pNode = (*i).findNode(pId);
    if(pNode)
      return pNode;
  }
  return 0;
}

void QSnpNode::setSelected(bool _bSelected)
{ 
  bSelected = _bSelected;
}

void QSnpNode::setExpanded(bool _bExpanded)
{ 
  bExpanded = _bExpanded; 
  if(pWidgetItem)
    pWidgetItem->setExpanded(bExpanded); 
}


// загрузка состояния вершины
void QSnpNode::loadState(bool bChildren)
{
  QSettings sett("QSnap");
  QString sTreeViewId = getInstance()->getTreeView()->getId();
  QString sGroup = QString("/")+=QString(sTreeViewId)+=QString("/Node/")+=id();

  sett.beginGroup(sGroup);
  bSelected = sett.value(QString("selected")).toBool();
  bExpanded = sett.value(QString("expanded")).toBool();
  sett.endGroup();

  setSelected(bSelected);
  QSnpTreeView* pView = getInstance() ? getInstance()->getTreeView() : 0;
  if(pView && pView->getTreeWidget())
    pView->getTreeWidget()->selectItem(pWidgetItem, bSelected, false);

  setExpanded(bExpanded);

  if(bChildren)
  {
    for(QList<QSnpNode>::iterator it=lstChildNode.begin(); it!=lstChildNode.end(); it++)
      it->loadState(bChildren);
  }
}

// сохранение состояния вершины
void QSnpNode::saveState(bool bChildren)
{
  QSettings sett("QSnap");
  QString sTreeViewId = getInstance()->getTreeView()->getId();
  QString sGroup = QString("/")+=QString(sTreeViewId)+=QString("/Node/")+=id();

  sett.beginGroup(sGroup);
  sett.setValue(QString("selected"), isSelected());
  sett.setValue(QString("expanded"), isExpanded());
  sett.endGroup();

  if(bChildren)
  {
    for(QList<QSnpNode>::iterator it=lstChildNode.begin(); it!=lstChildNode.end(); it++)
      it->saveState(bChildren);
  }
}
