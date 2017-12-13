/**
  \file   QSnpListView.cpp
  \brief  QSnpListView class represents View to show list
  \author Sholomov D.
  \date   30.08.2016
*/

#include "QSnpListView.h"
#include "QSnpListWidget.h"

#include <QScrollBar>
#include <QString>

using namespace QSnp;

QSnpListView::QSnpListView(void)
{
  pWidget = NULL;
  eViewType = VT_LIST_VIEW;
}

QSnpListView::~QSnpListView(void)
{
  Destroy();
}

QHandle QSnpListView::Create(QSnpInstance* _pInstance)
{
  if(pWidget)
    return (QHandle)this;
  
  QSnpListWidget* pWidget = new QSnpListWidget();
  pWidget->resize(500,300);
  pWidget->show();
  
  QSnpView::pWidget = pWidget;

  return (QHandle)this;
}

bool QSnpListView::Destroy(void)
{
  if(!pWidget)
    return true;
  delete pWidget;
  pWidget = 0;
  return true;
}

// добавление элемента в конец списка
int QSnpListView::AddItem(const char* text, void* data)
{
  if(!pWidget)
    return -1;
  QSnpListWidget* pWidget = (QSnpListWidget*)QSnpView::pWidget;
  pWidget->addItem(QString(text));

  QListWidgetItem * pItem = pWidget->item(pWidget->count()-1);
  if(!pItem)
    return -1;
  QVariant vData((long long)data);
  pItem->setData(1, vData);

  return Count()-1;
}

// вставка элемента после текущего
int QSnpListView::InsertItem(int rowAfter, const char* text, void* data)
{
  if(!pWidget)
    return -1;
  QSnpListWidget* pWidget = (QSnpListWidget*)QSnpView::pWidget;
  pWidget->insertItem(rowAfter, QString(text));
  
  QListWidgetItem * pItem = pWidget->item(rowAfter+1);
  if(!pItem)
    return -1;
  QVariant vData((long long)data);
  pItem->setData(1, vData);

  return rowAfter+1;
}

// удаление элемента
void QSnpListView::RemoveItem(int row)
{
  if(!pWidget)
    return;
  QSnpListWidget* pWidget = (QSnpListWidget*)QSnpView::pWidget;
  pWidget->removeItemWidget(pWidget->takeItem(row));
}


// добавление набора элементов в конец списка
int QSnpListView::AddItems(const char** texts)
{
  while(*texts)
    AddItem(*texts++);
  return Count()-1;
}

// установка текста для элемента
void QSnpListView::SetItemText(int row, const char* text)
{
  if(!pWidget)
    return;
  QSnpListWidget* pWidget = (QSnpListWidget*)QSnpView::pWidget;
  QListWidgetItem * pItem = pWidget->item(row);
  pItem->setText(QString(text));
}

// получение текста для элемента
const char* QSnpListView::GetItemText(int row)
{
  if(!pWidget)
    return 0;
  QSnpListWidget* pWidget = (QSnpListWidget*)QSnpView::pWidget;
  QListWidgetItem * pItem = pWidget->item(row);
  sBuffer = pItem->text().toStdString();
  return sBuffer.c_str();
}

// установка данного для элемента
void QSnpListView::SetItemData(int row, void* data)
{
  if(!pWidget)
    return;
  QSnpListWidget* pWidget = (QSnpListWidget*)QSnpView::pWidget;
  QListWidgetItem * pItem = pWidget->item(row);
  QVariant vData((long long)data);
  pItem->setData(1, vData);
}

// получение данного для элемента
void* QSnpListView::GetItemData(int row)
{
  if(!pWidget)
    return 0;
  QSnpListWidget* pWidget = (QSnpListWidget*)QSnpView::pWidget;
  QListWidgetItem * pItem = pWidget->item(row);
  QVariant vData = pItem->data(1);
  return (void*)vData.toLongLong();
}

// количество элементов
int QSnpListView::Count() const
{
  if(!pWidget)
    return -1;
  QSnpListWidget* pWidget = (QSnpListWidget*)QSnpView::pWidget;
  return pWidget->count();
}

// удалить все элементы
void QSnpListView::Clear()
{
  QSnpListWidget* pWidget = (QSnpListWidget*)QSnpView::pWidget;
  pWidget->clear();
}
  
// номер элемента с данным текстом
int QSnpListView::Row(const char* text, int rowStart) const
{
  if(!pWidget)
    return -1;
  QSnpListWidget* pWidget = (QSnpListWidget*)QSnpView::pWidget;
  QString qText(text);
  int nCount = Count();
  for(int nRow = rowStart; nRow<nCount; nRow++)
  {  
    QListWidgetItem * pItem = pWidget->item(nRow);
    if(pItem->text() == qText)
      return nRow;
  }
  return -1;
}

// номер селектированного элемента
int QSnpListView::GetCurrentRow()
{
  if(!pWidget)
    return -1;
  QSnpListWidget* pWidget = (QSnpListWidget*)QSnpView::pWidget;
  return pWidget->currentRow();
}

// установить селекцию на элемент с номером raw
void QSnpListView::SetCurrentRow(int row)
{
  if(!pWidget)
    return;
  QSnpListWidget* pWidget = (QSnpListWidget*)QSnpView::pWidget;
  pWidget->setCurrentRow(row);
}

// сортировка элементов допустима
void QSnpListView::SetSortingEnabled(bool enable)
{
  if(!pWidget)
    return;
  QSnpListWidget* pWidget = (QSnpListWidget*)QSnpView::pWidget;
  pWidget->setSortingEnabled(enable);
}

// сортировать элементы
void QSnpListView::SortItems(bool orderAcsending)
{
  if(!pWidget)
    return;
  QSnpListWidget* pWidget = (QSnpListWidget*)QSnpView::pWidget;
  pWidget->sortItems(orderAcsending ? Qt::AscendingOrder : Qt::DescendingOrder);
}

