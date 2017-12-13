/**
  \file   QSnpListView.h
  \brief  QSnpListView class represents View to show list
  \author Sholomov D.
  \date   30.08.2016
*/

#include <qsnap/qsnap.h>
#include "QSnpView.h"

#include <string>

class QSnpListView :
  public QSnpView
{
public:
  QSnpListView(void);
  virtual ~QSnpListView(void);

  /// создание окна
  virtual QHandle Create(QSnpInstance* _pInstance);

  /// разрушение окна
  virtual bool Destroy();

  // добавление элемента в конец списка
  int AddItem(const char* text, void* data = 0);

  // вставка элемента после текущего
  int InsertItem(int rowAfter, const char* text, void* data = 0);

  // удаление элемента
  void RemoveItem(int row);

  // добавление набора элементов в конец списка
  int AddItems(const char** texts);

  // установка текста для элемента
  void SetItemText(int row, const char* text);

  // получение текста для элемента
  const char* GetItemText(int row);

  // установка данного для элемента
  void SetItemData(int row, void* data);

  // получение данного для элемента
  void* GetItemData(int row);

  // количество элементов
  int Count() const;

  // удалить все элементы
  void Clear();
  
  // номер элемента с данным текстом
  int Row(const char* text, int rowStart = 0) const;

  // номер селектированного элемента
  int GetCurrentRow();

  // установить селекцию на элемент с номером raw
  void SetCurrentRow(int row);

  // сортировка элементов допустима
  void SetSortingEnabled(bool enable);

  // сортировать элементы
  void SortItems(bool orderAcsending = true);

protected:

  std::string sBuffer;
};
