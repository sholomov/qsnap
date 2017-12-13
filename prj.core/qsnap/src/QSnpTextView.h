/**
  \file   QSnpTextView.h
  \brief  QSnpTextView class represents View to show text Log
  \author Sholomov D.
  \date   15.5.2013
*/

#include <qsnap/qsnap.h>
#include "QSnpView.h"

class QSnpTextView :
  public QSnpView
{
public:
  QSnpTextView(void);
  virtual ~QSnpTextView(void);

  /// создание окна
  virtual QHandle Create(QSnpInstance* _pInstance);

  /// разрушение окна
  virtual bool Destroy();

  /// вывод лога
  void Log(const char* str);

  /// очистка текста
  void Clear();

 // virtual QWidget* getWidget() { return pWidget; }

protected:
 // QSnpTextEdit* pWidget;


};
