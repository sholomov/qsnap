/**
  \file   QSnpTextEdit.h
  \brief  QWidget based class to show text Log
  \author Sholomov D.
  \date   15.5.2013
*/

#pragma once

#include <QTextEdit>

class QSnpTextEdit : public QTextEdit
{
  Q_OBJECT
public:
  QSnpTextEdit(void);
  ~QSnpTextEdit(void);
};
