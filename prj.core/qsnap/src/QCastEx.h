/**
  \file   QCastEx.h
  \brief  Extra type casting functions
  \author Sholomov D.
  \date   28.5.2013
*/

#pragma once

#include <QRect>
#include <QPainter>
#include <qsnap/qsnap_types.h>

inline QColor QColorRGB_cast(int rgb_color)
{
  return QColor(rgb_color);
}

inline QColor QColorBGR_cast(int rgb_color)
{
  return QColor(
    (rgb_color & 0x00ff0000) >> 16,
    (rgb_color & 0x0000ff00) >> 8,
     rgb_color & 0x000000ff
    );
}

#define QColor_cast     QColorRGB_cast
#define QColorInv_cast  QColorBGR_cast

inline QSnp::SnpRect SnpRect_cast(QRect qrc)
{
  QSnp::SnpRect rc = {qrc.x(), qrc.y(), qrc.width(), qrc.height()};
  return rc;
}

inline QRect QRect_cast(QSnp::SnpRect rc)
{
  QRect qrc(rc.x, rc.y, rc.width, rc.height);
  return qrc;
}

inline float float_scale(float f, float ratio)
{
  return f*ratio+0.5;
}

inline QPoint QPoint_scale(QPoint _pt, float ratio)
{
  QPoint pt(
    float_scale(_pt.x(),ratio), 
    float_scale(_pt.y(),ratio)
    ); 
  return pt;
}

inline QRect QRect_scale(QRect _rc, float ratio)
{
  QRect rc(
    float_scale(_rc.x(),ratio), 
    float_scale(_rc.y(),ratio),
    float_scale(_rc.width(),ratio),
    float_scale(_rc.height(),ratio)
    ); 
  return rc;
}

