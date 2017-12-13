/**
  \file   QSnpTreeWidget.cpp
  \brief  Functions for QWidget based class to show and interact with QSnap control Tree
  \author Sholomov D.
  \date   15.5.2013
*/

#include "QSnpTreeWidget.h"
#include "QSnpNode.h"

#include <QCoreApplication>
#include <QKeyEvent>
#include <QEvent>

using namespace QSnp;

//////////////////////////////////////////////////////////////////////////////
//// Класс StwEventFilter Фильтр событий для перехвата событий аппликации

class StwEventFilter : public QObject
{
public:
  StwEventFilter(QObject* obj = 0) : QObject(obj) { pWidget=static_cast<QSnpTreeWidget*>(obj); }

protected:
  // основная функция фильтрации событий
  virtual bool eventFilter(QObject *obj, QEvent *event)
  {
    if ( event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease )
    {
      QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
      if(pWidget)
        pWidget->kbdModifiers = keyEvent->modifiers();
      return false;
    }
    else return QObject::eventFilter(obj, event);        // standard event processing
 }

private:
  class QSnpTreeWidget* pWidget;  // класс, реагирующий на события аппликации

};

//////////////////////////////////////////////////////////////////////////////
//// Класс QSnpTreeWidget 

QSnpTreeWidget::QSnpTreeWidget(void)
{
  connect(
    this, SIGNAL(itemDoubleClicked( QTreeWidgetItem * , int  )), 
    this, SLOT(onItemDoubleClicked( QTreeWidgetItem * , int  ))
    );

  setExpandsOnDoubleClick(false);

  qApp->installEventFilter(new StwEventFilter(this));
}

QSnpTreeWidget::~QSnpTreeWidget(void)
{
}

QSnpNode* QSnpTreeWidget::getQSnpNode(QTreeWidgetItem * item, int column )
{
  QVariant vData = item->data(column, TID_QNODE_REF);
  QSnpNode* pNode = (QSnpNode*)(vData.toULongLong());
  return pNode;
}

QSnpNode* QSnpTreeWidget::getCurrentQSnpNode()
{
  QTreeWidgetItem * item = currentItem();

  if(!item)
    return NULL;

  QVariant vData = item->data(0, TID_QNODE_REF);
  QSnpNode* pNode = (QSnpNode*)(vData.toULongLong());
  return pNode;
}

void QSnpTreeWidget::setQSnpNode(QSnpNode* pNode, QTreeWidgetItem * item, int column )
{
  QVariant qData((QHandle)pNode);
  item->setData(column, TID_QNODE_REF, qData);
}

void QSnpTreeWidget::onItemDoubleClicked(QTreeWidgetItem * item, int column )
{
  QVariant vData = item->data(0, TID_QNODE_REF);
  QSnpNode* pNode = (QSnpNode*)(vData.toULongLong());
  bool bSelected = pNode->isSelected();
  bSelected = !bSelected;
  bool bChildren = kbdModifiers & Qt::ControlModifier;
  selectItem(item, bSelected, bChildren);
}

void QSnpTreeWidget::selectItem(QTreeWidgetItem * item, bool bSelect, bool bChildren)
{
  QVariant vData = item->data(0, TID_QNODE_REF);
  QSnpNode* pNode = (QSnpNode*)(vData.toULongLong());
  pNode->setSelected(bSelect);

  QFont font = item->font(0);
  font.setBold(bSelect);
  item->setFont(0,font);

  // селекция дочерних вершин
  if(bChildren)
  {
    for(int nChild=0; nChild<item->childCount(); nChild++)
    {
      QTreeWidgetItem * pChildItem = item->child(nChild);
      selectItem(pChildItem, bSelect, bChildren);
    }
  }
}

void QSnpTreeWidget::expandItem(QTreeWidgetItem * item, bool bExpand, bool bChildren)
{
  QVariant vData = item->data(0, TID_QNODE_REF);
  QSnpNode* pNode = (QSnpNode*)(vData.toULongLong());
  pNode->setExpanded(bExpand);

  item->setExpanded(bExpand);

  // селекция дочерних вершин
  if(bChildren)
  {
    for(int nChild=0; nChild<item->childCount(); nChild++)
    {
      QTreeWidgetItem * pChildItem = item->child(nChild);
      expandItem(pChildItem, bExpand, bChildren);
    }
  }
}
