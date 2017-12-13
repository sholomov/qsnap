/**
  \file   qsnap_cast.h
  \brief  Functions for QSnap-types casting Majority of castings are OpenCV->QSnap
  \author Sholomov D.
  \date   15.08.2016
*/

#pragma once

#ifndef QSNAP_CAST_H
#define QSNAP_CAST_H

#include <qsnap/qsnap_types.h>
#include <opencv2/core.hpp>

#ifndef RGB
  #define RGB(r,g,b) ( (unsigned char)(r) | ((unsigned char)(g)<<8) | ((unsigned char)(b)<<16) )
#endif

namespace QSnp {

// cv::Rect -> QSnp::SnpRect
inline QSnp::SnpRect SnpRect_cast(cv::Rect _rc)
{
  QSnp::SnpRect rc = {_rc.x, _rc.y, _rc.width, _rc.height};
  return rc;
}

// cv::Point -> QSnp::SnpPoint
inline QSnp::SnpPoint SnpPoint_cast(cv::Point _pt)
{
  QSnp::SnpPoint pt = {_pt.x, _pt.y};
  return pt;
}

// QSnp::SnpRect -> cv::Rect
inline cv::Rect cvRect_cast(QSnp::SnpRect rc)
{
  return cv::Rect(rc.x, rc.y, rc.width, rc.height);
}

// QSnp::SnpPoint -> cv::Point
inline cv::Point cvPoint_cast(QSnp::SnpPoint pt)
{
  return cv::Point(pt.x, pt.y);
}

// Преобразование из модели пикселя OpenCV (b,g,r)
inline QSnp::SnpColor Color_cast(cv::Scalar _col)
{
  QSnp::SnpColor col = RGB(_col[0],_col[1],_col[2]);
  return col;
}

// Преобразование из модели RGB
inline QSnp::SnpColor Color_cast(unsigned char r, unsigned char g, unsigned char b)
{
  QSnp::SnpColor col = RGB(r,g,b);
  return col;
}

// QSnp::SnpRect -> std::string
inline std::string string_cast(QSnp::SnpRect rc)
{
  char buff[256]={0};
  sprintf(buff,"(%d,%d,%d,%d)", rc.x, rc.y, rc.width, rc.height);
  return std::string(buff);
}

// QSnp::SnpPoint -> std::string
inline std::string string_cast(QSnp::SnpPoint pt)
{
  char buff[256]={0};
  sprintf(buff, "(%d,%d)", pt.x, pt.y);
  return std::string(buff);
}

// cv::Rect -> QSnp::SnpRect
inline QSnp::SnpRect SnpRect_scast(cv::Rect _rc, double scale)
{
  QSnp::SnpRect rc = {
    static_cast<int>(_rc.x*scale), 
    static_cast<int>(_rc.y*scale), 
    static_cast<int>(_rc.width*scale),
    static_cast<int>(_rc.height*scale)
  };
  return rc;
}

};  // namespace QSnp

#endif // QSNAP_CAST_H
