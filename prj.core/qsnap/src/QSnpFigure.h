/**
  \file   QSnpFigure.h
  \brief  Polymorph classes for drawing figures in QImageView
  \author Sholomov D.
  \date   15.5.2013
*/

#pragma once

#include <QRect>
#include <QPainter>
#include "QCastEx.h"

#define SFT_NONE      0
#define SFT_POINT     1
#define SFT_LINE      2
#define SFT_RECT      3
#define SFT_ELLIPSE   4
#define SFT_TEXT      5


class QSnpFigure
{
public:
  QSnpFigure(class QSnpView* pView=0);
  virtual ~QSnpFigure(void);

  virtual int getType() { return SFT_NONE; }
  virtual void Draw(QPainter* pPainter, float ratio)=0;

  QRect rc;
  QColor color;
  int lineWidth;
  bool bVisible;

  void setVisible(bool bVisible);

  QSnpView* getView() { return pView; }
  void setView(QSnpView* p) { pView = p; }

private:
  QSnpView* pView;

};


///////////////////////////////////////////////////////////
////  Классы, соответствующие фигурам

class QSnpLine : public QSnpFigure
{
public:
  QSnpLine(QSnpView* pView=0);
  virtual ~QSnpLine(void);

  virtual int getType() { return SFT_LINE; }
  virtual void Draw(QPainter* pPainter, float ratio = 1.0);

public:
  QPoint ptFrom;
  QPoint ptTo;

};

class QSnpRect : public QSnpFigure
{
public:
  QSnpRect(QSnpView* pView=0);
  virtual ~QSnpRect(void);

  virtual int getType() { return SFT_RECT; }
  virtual void Draw(QPainter* pPainter, float ratio = 1.0);
};

class QSnpEllipse : public QSnpFigure
{
public:
  QSnpEllipse(QSnpView* pView=0);
  virtual ~QSnpEllipse(void);

  virtual int getType() { return SFT_ELLIPSE; }
  virtual void Draw(QPainter* pPainter, float ratio = 1.0);
};

class QSnpText : public QSnpFigure
{
public:
  QSnpText(QSnpView* pView=0);
  virtual ~QSnpText(void);

  virtual int getType() { return SFT_TEXT; }
  virtual void Draw(QPainter* pPainter, float ratio = 1.0);

public:
  QString textValue;

  double  fontSize;
  QString fontType;
  bool    bBold;
  bool    bItalic;
  int     alignment;

};

class QSnpPoint : public QSnpFigure
{
public:
  QSnpPoint(QSnpView* pView=0, int x=0, int y=0, int lineWidth=1);
  virtual ~QSnpPoint(void);

  virtual int getType() { return SFT_POINT; }
  virtual void Draw(QPainter* pPainter, float ratio = 1.0);

  virtual int x() { return mX; }
  virtual int y() { return mY; }

  protected:
    int mX;
    int mY;


};