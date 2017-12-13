/**
  \file   QSnpNode.h
  \brief  QSnpNode class represents Node object in QSnpTreeView 
  \author Sholomov D.
  \date   15.5.2013
*/

#pragma once

#include <qsnap/qsnap.h>

#include <QString>
#include <QList>
#include <QVector>
#include <QTreeWidget>

class QSnpInstance;
class QSnpRootNode;

class QSnpNode
{
public:

  // конструктор/деструктор
  QSnpNode(const char* pId=NULL, const char* pCaption=NULL);
  virtual ~QSnpNode();

  // получение/установка инстенса снепа
  QSnpInstance* getInstance() { return pInstance; } 
  void setInstance(QSnpInstance* _pInstance) { pInstance=_pInstance; }

  // добавление дочерней вершины
  QSnpNode* addChildNode(QSnp::NodeInfo& nodeInfo);
  
  // получение/установка виджета древа снепа
  QTreeWidgetItem* getTreeWidgetItem() { return pWidgetItem; }
  void setTreeWidgetItem(QTreeWidgetItem* _pWidgetItem) { pWidgetItem=_pWidgetItem; }

  // информация о селекции
  bool isSelected() { return bSelected; }
  void setSelected(bool _bSelected);
 
  // информация о раскрытии
  bool isExpanded() { return pWidgetItem ? pWidgetItem->isExpanded() : false; }
  void setExpanded(bool _bExpanded);

  // получение/установка ID вершины
  QString id() { return sId; }
  void setId(const char* pId) { sId = pId; }

  // получение/установка заголовка
  QString caption() { return sCaption; }
  void setCaption(const char* pCaption) { sCaption = pCaption; }

  // хэндл вершины
  QHandle handle() { return (QHandle)this; }

  // поиск вершины с данным id (первой попавшейся)
  QSnpNode* findNode(const char* pId);

  // загрузка состояния вершины
  void loadState(bool bChildren = false);

  // сохранение состояния вершины
  void saveState(bool bChildren = false);

public: // Node Properties
  QVector<QSnp::NodePropertyInfo> vProperties;

protected:
  QString sId;
  QString sCaption;
  bool bSelected;
  bool bExpanded;

  QList<QSnpNode> lstChildNode;
  QSnpRootNode* pRootNode;

protected:
  QSnpInstance* pInstance;
  QTreeWidgetItem* pWidgetItem;

};
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
const int TID_QNODE_REF = 15;

class QSnpRootNode : public QSnpNode
{
public:
  QSnpRootNode() : QSnpNode("Root", "Root") { ; }
  virtual ~QSnpRootNode() { ; }
};
