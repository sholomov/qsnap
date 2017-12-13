/**
  \file   qsnap_tuple.h
  \brief  Snap-context minimal structure to be passed through functions using QSnap
  \author Dmitry Sholomov
  \date   11.08.2016
*/

#pragma once
#ifndef QSNAP_TUPLE_H
#define QSNAP_TUPLE_H

#include <qsnap/qsnapx.h>

#ifdef __GCC__
  #pragma GCC diagnostic ignored "-Weffc++"
#endif // __GCC__

///////////////////////////////////////////////////////////////////////
////  QSnapTuple structure

struct QSnapTuple
{
  QSpxInstance*   snpInstance;            /// инстенс снепа singleton
  QSpxImageView*  imageView;              /// визуализация изображения (с левой камеры)
  QSpxTextView*   logView;                /// класс текстового лога
  QSpxTreeView*   treeView;               /// класс для работы с древом узлов снепа
  QSpxNode*       nodeParent;             /// родительский узел дерева
};

#endif // QSNAP_TUPLE_H
