/**
  \file   qsnap.h
  \brief  QSnap library application programming interface
  \author Sholomov D.
  \date   15.5.2013
*/

#pragma once

#ifndef QSNAP_H
#define QSNAP_H

#include <qsnap/qsnap_types.h>
#include <qsnap/qsnap_global.h>

#include <opencv2/core/core.hpp>

#ifdef __MINIMG__
#include <minutils/minimg.h>
#endif

namespace QSnp {
extern "C" {
  
////////////////////////////////////////////////////////
//////    Функции инициализации
////////////////////////////////////////////////////////

// инициализация инстанса снэпа
QSNAP_API QHandle Initialize          // [ret] хэндл экземпляра snap
(
  bool bThreadMode                    // [in]  режим отдельного потока всех вызовов или нет, true - да
);

// деинициализация инстанса снэпа
QSNAP_API QError Terminate
(
  QHandle hInstance                   // [in]  хэндл экземпляра snap
);

////////////////////////////////////////////////////////
//////    Работа с окнами Views
////////////////////////////////////////////////////////

// создание окон просмотра (редактирования) определенного типа
QSNAP_API QHandle CreateView          // [ret] хэндл окна
(
  QHandle hInstance,                  // [in]  хэндл экземпляра snap
  QSnp::ViewType eViewType,           // [in]  тип окна просмотра
  const char* sId,                    // [in]  идентификатор окна   
  long long parm                      // [in]  дополнительный параметр
);

// уничтожение окона просмотра
QSNAP_API QError DestroyView
(
  QHandle hView                       // [in]  хэндл окна
);

// поиск окна просмотра по идентификатору
QSNAP_API QHandle FindView            // [ret] хэндл окна
(
  QHandle hInstance,                  // [in]  хэндл экземпляра snap
  const char* sId                     // [in]  идентификатор окна   
);

// показ/скрытие окна просмотра
QSNAP_API QError ShowView
(
  QHandle hView,                      // [in]  хэндл окна
  bool    bShow                       // [in]  показать/скрыть
);

// перерисовка прямоугольной зоны окна
QSNAP_API QError UpdateView
(
  QHandle   hView,                     // [in]  хэндл окна
  SnpRect*  pRect                      // [in]  зона для перерисовки
);

// получение общих свойств окна (координаты и проч.)
QSNAP_API QError GetViewInfo
(
  QHandle   hView,                    // [in]  хэндл окна
  ViewInfo* pViewInfo                 // [out] свойства окна
);

// установка общих свойств окна (координаты и проч.)
QSNAP_API QError SetViewInfo
(
  QHandle   hView,                    // [in] хэндл окна
  ViewInfo* pViewInfo                 // [in] свойства окна
);

QSNAP_API QHandle GetInstance         // [ret] хэндл инстанса
(
  QHandle   hView                     // [in] хэндл окна
);

////////////////////////////////////////////////////////
//////    Работа с конфигурациями окон просмотра
////////////////////////////////////////////////////////

// загрузка конфигурации
QSNAP_API QError LoadViewConfig
(
  QHandle hInstance,                  // [in] хэндл экземпляра snap
  const char* sConfigName             // [in] название конфигурации
);

// сохранение текущей конфигурации
QSNAP_API QError SaveCurrentViewConfig
(
  QHandle hInstance,                  // [in] хэндл экземпляра snap
  const char* sConfigName             // [in] название конфигурации
);

// закрытие всех окон
QSNAP_API QError DestroyAllViews
(
   QHandle hInstance                  // [in] хэндл экземпляра snap
);

// показ/сокрытие всех окон
QSNAP_API QError ShowAllViews
(
  QHandle hInstance,                  // [in] хэндл экземпляра snap
  bool bShow                          // [in] показать/скрыть
);

// перерисовка всех окон
QSNAP_API QError UpdateAllViews
(
  QHandle hInstance                   // [in] хэндл экземпляра snap
);

////////////////////////////////////////////////////////
//////    Работа с древом управления
////////////////////////////////////////////////////////

// получение корневой вершины
QSNAP_API QSnapNode GetSnapTreeRoot   // [ret] хэндл корневой вершины
(
  QHandle hInstance                   // [in]  хэндл экземпляра snap
);

// добавление вершины
QSNAP_API QSnapNode AddNode           // [ret] хэндл новой вершины
(
  QSnapNode       hParentNode,        // [in]  хэндл родительской вершины
  QSnp::NodeInfo* pNodeInfo           // [in]  свойства вершины
);

// удаление вершины
QSNAP_API QError RemoveNode
(
  QSnapNode hNode                     // [in]  хэндл удаляемой вершины
);

// получение свойств вершины (см. NodeInfo)
QSNAP_API QError GetNodeInfo
(
  QSnapNode hNode,                    // [in]  хэндл вершины
  QSnp::NodeInfo* nodeInfo            // [out] свойства вершины
);

// установка свойств вершины (см. NodeInfo)
QSNAP_API QError SetNodeInfo
(
  QSnapNode hNode,                    // [in]  хэндл вершины
  QSnp::NodeInfo* nodeInfo            // [in]  свойства вершины
);

// поиск вершины по текстовому ID
QSNAP_API QSnapNode FindNode
(
  QHandle hSnapView,                  // [in]  хэндл окна
  const char* sNodeId                 // [in]  текстовый id вершины
);

// проверка выделена ли вершина
QSNAP_API bool NodeChecked            // [ret] выделена вершина или нет
(
  QSnapNode hNode                     // [in]  хэндл вершины
);

// выделение вершины
QSNAP_API QError CheckNode
(
  QSnapNode hNode,                    // [in]  хэндл вершины
  bool bCheck                         // [in]  выделить/убрать выделение
);

// получение вершины - родителя
QSNAP_API QSnapNode GetParentNode     // [ret] хэндл родительской вершины
(
  QSnapNode hNode                     // [in]  хэндл вершины
);

// получение вершины - первого ребенка
QSNAP_API QSnapNode GetFirstChildNode // [ret] хэндл первого ребенка
(
  QSnapNode hNode                     // [in]  хэндл вершины
);

// получение вершины - следующего ребенка
QSNAP_API QSnapNode GetNextChildNode // [ret] хэндл следующего ребенка
(
  QSnapNode hNodeParent,             // [in]  хэндл вершины-родителя
  QSnapNode hNodeCurrChild           // [in]  хэндл текущего ребенка
);

// получение вершины - предыдущего ребенка
QSNAP_API QSnapNode GetPrevChildNode // [ret] хэндл предыдущего ребенка
(
  QSnapNode hNodeParent,             // [in]  хэндл вершины-родителя
  QSnapNode hNodeCurrChild           // [in]  хэндл текущего ребенка
);

// загрузить состояние вершин
QSNAP_API void LoadTreeViewState
(
  QHandle       hView                // [in]  хэндл окна
);

// сохранить состояние вершин
QSNAP_API void SaveTreeViewState
(
  QHandle       hView                // [in]  хэндл окна
);

////////////////////////////////////////////////////////
//////    Работа со свойствами вершин
////////////////////////////////////////////////////////

// установка свойства вершины
QSNAP_API int GetNodePropertiesCount // [ret] количество свойств
(
  QSnapNode hNode                    // [in]  хэндл вершины
);

// получение свойства вершины
QSNAP_API QError GetNodeProperty
(
  QSnapNode         hNode,           // [in]  хэндл вершины
  int               nProperty,       // [in]  номер свойства
  NodePropertyInfo* pNodeProperty    // [out] свойство
);

// установка свойства вершины, значение свойств сохраняются между сеансами
QSNAP_API QError SetNodeProperty
(
  QSnapNode         hNode,           // [in]  хэндл вершины
  int               nProperty,       // [in]  номер свойства
  NodePropertyInfo* pNodeProperty    // [in]  свойство
);

// получение свойства вершины
QSNAP_API const char* GetNodeValue   // [ret] значение свойства, NULL если свойство не существует
(
  QSnapNode hNode,                   // [in]  хэндл вершины
  const char* sKey                   // [in]  название свойства
);

// установка свойства вершины
QSNAP_API QError SetNodeValue
(
  QSnapNode hNode,                   // [in]  хэндл вершины
  const char* sKey,                  // [in]  название свойства
  const char* sValue                 // [in]  значение свойства
);

// удаление свойства
QSNAP_API QError RemoveNodeProperty
(
  QSnapNode hNode,                   // [in]  хэндл вершины
  const char* sKey                   // [in]  название свойства
);

// удаление всех свойств вершины
QSNAP_API QError RemoveAllNodeProperties
(
  QSnapNode hNode                    // [in]  хэндл вершины
);

// загрузка свойств вершины
QSNAP_API QError LoadNodeProperties
(
  QSnapNode hNode                    // [in]  хэндл вершины
);

// сохранение свойств вершины
QSNAP_API QError SaveNodeProperties
(
  QSnapNode hNode                    // [in]  хэндл вершины
);

// загрузка свойства вершины
QSNAP_API QError LoadNodeProperty
(
  QSnapNode hNode,                   // [in]  хэндл вершины
  const char* sKey                   // [in]  название свойства
);

// сохранение свойства вершины
QSNAP_API QError SaveNodeProperty
(
  QSnapNode hNode,                   // [in]  хэндл вершины
  const char* sKey                   // [in]  название свойства
);

////////////////////////////////////////////////////////
//////    Работа с окном лога QTextView
////////////////////////////////////////////////////////

// получение свойств текстового окна (см. TextViewInfo)
QSNAP_API QError GetTextViewInfo
(
  QHandle   hView,                   // [in]  хэндл окна
  TextViewInfo* pViewInfo            // [out] свойства текстового окна 
);

// установка свойств текстового окна (см. TextViewInfo)
QSNAP_API QSnapNode SetTextViewInfo
(
  QHandle   hView,                   // [in]  хэндл окна
  TextViewInfo* pViewInfo            // [in]  свойства текстового окна 
);

// вывод текстового сообщения
QSNAP_API QError Log
(
  QHandle     hView,                 // [in]  хэндл окна
  const char* sText,                 // [in]  текстовый шаблон
  ...                                // [in]  параметры шаблона
);

// вывод текстового сообщения в виде отдельной строки
QSNAP_API QError LogLn
(
  QHandle     hView,                 // [in]  хэндл окна
  const char* sText,                 // [in]  текстовый шаблон
  ...                                // [in]  параметры шаблона
);

// вывод расширенного текстового сообщения
QSNAP_API QError LogEx
(
  QHandle     hView,                 // [in]  хэндл окна
  const char* sText,                 // [in]  текстовый шаблон
  SnpColor    color,                 // [in]  цвет текста
  FontFlags   flags                  // [in]  параметры шрифта
);

// очистка текста в текстовом окне
QSNAP_API QError ClearTextView
(
  QHandle     hView                  // [in]  хэндл окна
);

////////////////////////////////////////////////////////
//////    Работа с окном показа изображения QImageView
////////////////////////////////////////////////////////

// установка изображения
QSNAP_API QError SetImage
(
  QHandle     hView,                 // [in]  хэндл окна
  const char* sFileName,             // [in]  файл изображения
  ImageFlags  flagsShow              // [in]  флаги показа изображения
);

// установка изображения cv::Mat
QSNAP_API QError SetMatImage
(
  QHandle         hView,              // [in]  хэндл окна
  const cv::Mat*  pImage,             // [in]  изображение cv::Mat
  ImageFlags      flagsShow           // [in]  флаги показа изображения
);

// установка изображения в случае нескольких окон показа
QSNAP_API QError SetSubImage
(
  QHandle     hView,                 // [in]  хэндл окна
  const char* sFileName,             // [in]  файл изображения
  int         nSubView,              // [in]  номер внутреннего окна (для SynqImageView и проч.)
  ImageFlags  flagsShow              // [in]  флаги показа изображения
);

// установка Mat-изображения в случае нескольких окон показа
QSNAP_API QError SetSubMatImage
(
  QHandle       hView,               // [in]  хэндл окна
  const cv::Mat*  pImage,            // [in]  изображение cv::Mat
  int           nSubView,            // [in]  номер внутреннего окна (для SynqImageView и проч.)
  ImageFlags    flagsShow            // [in]  флаги показа изображения
);

#ifdef __MINIMG__

// установка изображения хранимого в MinImg
QSNAP_API QError SetMinImage
(
  QHandle       hView,              // [in]  хэндл окна
  const MinImg* pMinImage,          // [in]  изображение
  ImageFlags    flagsShow           // [in]  флаги показа изображения
);

// установка изображения хранимого в MinImg в случае нескольких окон показа
QSNAP_API QError SetSubMinImage
(
  QHandle       hView,               // [in]  хэндл окна
  const MinImg* pMinImage,           // [in]  изображение
  int           nSubView,            // [in]  номер внутреннего окна (для SynqImageView и проч.)
  ImageFlags    flagsShow            // [in]  флаги показа изображения
);

#endif

// получение информации об изображении (см. ImageViewInfo)
QSNAP_API QError GetImageViewInfo
(
  QHandle         hView,              // [in]  хэндл окна
  ImageViewInfo*  pViewInfo           // [out] свойства окна
);

// установка информации об изображении  (см. ImageViewInfo)
QSNAP_API QError SetImageViewInfo
(
  QHandle         hView,              // [in]  хэндл окна
  ImageViewInfo*  pViewInfo           // [in]  свойства окна
);

// масштабирование изображения
QSNAP_API QError ImageScaleToRect
(
  QHandle       hView,              // [in]  хэндл окна
  SnpRect*      pRect               // [in]  прямоугольник, масштабируемый в размер окна
);

// другие функции работы с окном просмотра изображений
// ...


////////////////////////////////////////////////////////
//////    Работа с окном списка QListView
////////////////////////////////////////////////////////

// добавление элемента в конец списка
QSNAP_API int AddListViewItem
(
  QHandle       hView,               // [in]  хэндл окна
  const char*   text,                // [in]  текст элемента
  void*         data                 // [in]  данные элемента
);

// вставка элемента после текущего
QSNAP_API int InsertListViewItem
(
  QHandle       hView,               // [in]  хэндл окна
  int           rowAfter,            // [in]  после какого элемента вставить
  const char*   text,                // [in]  текст элемента
  void*         data                 // [in]  данные элемента
);

// удаление элемента
QSNAP_API int RemoveListViewItem
(
  QHandle       hView,               // [in]  хэндл окна
  int           row                  // [in]  какой элемент удалить
);

// добавление набора элементов в конец списка
QSNAP_API int AddListViewItems
(
  QHandle       hView,               // [in]  хэндл окна
  const char**  texts,               // [in]  указатели на тексты элементов
  void**        datas                // [in]  данные элементов
);

// установка текста для элемента
QSNAP_API int SetListViewItemText
(
  QHandle       hView,               // [in]  хэндл окна
  int           row,                 // [in]  номер элемента
  const char*   text                 // [in]  текст элемента
);

// получение текста для элемента
QSNAP_API const char* GetListViewItemText
(
  QHandle       hView,               // [in]  хэндл окна
  int           row                  // [in]  номер элемента
);

// установка данного для элемента
QSNAP_API void SetListViewItemData
(
  QHandle       hView,               // [in]  хэндл окна
  int           row,                 // [in]  номер элемента
  void*         data                 // [in]  данные элемента
);

// получение данного для элемента
QSNAP_API void* GetListViewItemData
(
  QHandle       hView,               // [in]  хэндл окна
  int           row                  // [in]  номер элемента
);

// количество элементов
QSNAP_API int GetListViewCount
(
  QHandle       hView                // [in]  хэндл окна
);

// удалить все элементы
QSNAP_API void ClearListView
(
  QHandle       hView                // [in]  хэндл окна
);

// номер элемента с данным текстом
QSNAP_API int GetListViewRow
(
  QHandle       hView,               // [in]  хэндл окна
  const char*   text,                // [in]  текст элемента
  int           rowStart             // [in]  начальный элемент поиска
);

// номер элемента с данным текстом
QSNAP_API int GetListViewRow
(
  QHandle       hView,               // [in]  хэндл окна
  const char*   text,                // [in]  текст элемента
  int           rowStart             // [in]  начальный элемент поиска
);

// номер селектированного элемента
QSNAP_API int GetListViewCurrentRow
(
  QHandle       hView                // [in]  хэндл окна
);

// установка селектированного элемента
QSNAP_API void SetListViewCurrentRow
(
  QHandle       hView,               // [in]  хэндл окна
  int           row                  // [in]  номер элемента
);

// номер элемента с данным текстом
QSNAP_API void SetListViewSortingEnabled
(
  QHandle       hView,               // [in]  хэндл окна
  bool          enable               // [in]  разрешить сортировку
);

// номер элемента с данным текстом
QSNAP_API void SortListViewItems
(
  QHandle       hView,               // [in]  хэндл окна
  bool          orderAcsending       // [in]  сортировка по возростанию / по убыванию 
);

////////////////////////////////////////////////////////
//////    Вывод пользовательской информации
////////////////////////////////////////////////////////

// установка коэффициента масштабирования фигур в координатах изображения
QSNAP_API QError SetScaleFactor
(
  QHandle       hView,              // [in] хэндл окна
  double        scale,              // [in] координата по горизонтали
  const char*   idGroup             // [in] идентификатор группы фигур (not implemented)
);

// отрисовка точки
QSNAP_API QHandle DrawPoint         // [ret] хэндл фигуры
(
  QHandle       hView,              // [in] хэндл окна
  int           x,                  // [in] координата по горизонтали
  int           y,                  // [in] координата по вертикали
  int           ptWidth,            // [in] толщина точки (диаметр)
  SnpColor      color,              // [in] цвет
  const char*   idGroup             // [in] идентификатор группы фигур (для совместной работы)
);

// отрисовка линии
QSNAP_API QHandle DrawLine          // [ret] хэндл фигуры
(
  QHandle       hView,              // [in] хэндл окна
  int           xFrom,              // [in] координата начала по горизонтали
  int           yFrom,              // [in] координата начала по вертикали
  int           xTo,                // [in] координата конца по горизонтали
  int           yTo,                // [in] координата конца по вертикали
  int           lineWidth,          // [in] толщина линии
  SnpColor      color,              // [in] цвет
  LineType      type,               // [in] тип линии (линия, стрелка, пунктир и т.д.)
  const char*   idGroup             // [in] идентификатор группы фигур
);

// отрисовка прямоугольника
QSNAP_API QHandle DrawRect          // [ret] хэндл фигуры
(
  QHandle       hView,              // [in] хэндл окна
  SnpRect*      pRect,              // [in] координаты прямоугольника
  int           lineWidth,          // [in] толщина линии
  SnpColor      color,              // [in] цвет
  LineType      type,               // [in] тип линии (линия, пунктир и т.д.)
  const char*   idGroup             // [in] идентификатор группы фигур
);

// отрисовка многоугольника, ломанной
QSNAP_API QHandle DrawPolygon       // [ret] хэндл фигуры
(
  QHandle       hView,              // [in] хэндл окна
  SnpPoint*     points,             // [in] массив точек многоугольника 
  int*          szPoints,           // [in,out] размер массива точек
  int           lineWidth,          // [in] толщина линии
  SnpColor      color,              // [in] цвет
  PolygonType   type                // [in] тип многоугольника (ломаная, замкнутый и т.д.)
);

// отрисовка эллипса
QSNAP_API QHandle DrawEllipse       // [ret] хэндл фигуры
(
  QHandle       hView,              // [in] хэндл окна
  SnpRect*      pRect,              // [in] координаты охватывающего прямоугольника
  int           lineWidth,          // [in] толщина линии
  SnpColor      color,              // [in] цвет
  LineType      type,               // [in] тип линии (линия, пунктир и т.д.)
  const char*   idGroup             // [in] идентификатор группы фигур
);

// отрисовка дуги
QSNAP_API QHandle DrawArc           // [ret] хэндл фигуры
(
  QHandle       hView,              // [in] хэндл окна
//  ...                             // [in] параметры дуги
  int           lineWidth,          // [in] толщина линии
  SnpColor      color,              // [in] цвет
  LineType      type,               // [in] тип линии (линия, пунктир и т.д.)
  const char*   idGroup             // [in] идентификатор группы фигур
);

// отрисовка текста
QSNAP_API QHandle DrawText          // [ret] хэндл текста
(
  QHandle       hView,              // [in] хэндл окна
  SnpRect*      pRect,              // [in] координаты охватывающего прямоугольника
  const char*   textValue,          // [in] текстовое значение
  double        fontSize,           // [in] кегль шрифта
  SnpColor      fontColor,          // [in] цвет шрифта
  const char*   fontType,           // [in] тип шрифта
  const char*   idGroup             // [in] идентификатор группы фигур
);

// функция показа/скрытия фигуры
QSNAP_API QError ShowFigure
(
  QHandle       hFigure,            // [in] хэндл фигуры
  bool          bShow               // [in] показать/скрыть
);

// функция удаления всех фигур в данном окне
QSNAP_API QError ClearFigures
(
  QHandle       hView               // [in] хэндл окна
);

// функция показа/скрытия группы фигур
QSNAP_API QError ShowGroup
(
  QHandle       hView,              // [in] хэндл окна
  const char*   idGroup,            // [in] id группы фигур
  bool          bShow               // [in] показать/скрыть
);

////////////////////////////////////////////////////////
//////    Работа с окном кадров QFrameView
////////////////////////////////////////////////////////

// установка фильма
QSNAP_API QError SetFrames
(
  QHandle     hView,                // [in] хэндл окна
  const char* sFileName,            // [in] файл фильма
  FrameFlags  flagsShow             // [in] флаги показа фильма (растягивать, центрировать и т.д.)
);

// получение информации о фильме (см. FrameViewInfo)
QSNAP_API QError GetFrameViewInfo
(
  QHandle         hView,            // [in]  хэндл окна
  FrameViewInfo*  pViewInfo         // [out] свойства окна
);

// установка информации о ролике (см. FrameViewInfo)
QSNAP_API QError SetFrameViewInfo
(
  QHandle         hView,            // [in]  хэндл окна
  FrameViewInfo*  pViewInfo         // [in]  свойства окна
);

// получение информации о кадре (см. FrameInfo)
QSNAP_API QError GetFrameInfo
(
  QHandle         hView,            // [in]  хэндл окна
  int             nFrame,           // [in]  номер кадра
  FrameInfo*      pFrameInfo        // [out] свойства фильма
);

// установка информации о кадре (см. FrameInfo)
QSNAP_API QError SetFrameInfo
(
  QHandle         hView,            // [in]  хэндл окна
  int             nFrame,           // [in]  номер кадра
  FrameInfo*      pFrameInfo        // [in]  свойства фильма
);

// количество кадров
QSNAP_API int GetFrameCount         // [ret] количество кадров, -1 в случае ошибки
(
  QHandle     hView                 // [in]  хэндл окна
);

// установить/убрать выделение кадра
QSNAP_API QError SelectFrame
(
  QHandle     hView,                // [in]  хэндл окна
  int         nFrame,               // [in]  номер кадра
  bool        bSelect               // [in]  выделить / убрать выделение
);

// выделен кадр или нет
QSNAP_API bool IsFrameSelected
(
  QHandle     hView,                // [in]  хэндл окна
  int         nFrame                // [in]  номер кадра
);

// получить выделенные кадры
QSNAP_API QError GetSelectedFrames
(
  QHandle     hView,                // [in]  хэндл окна
  int*        nFrames,              // [out] массив номеров кадров   
  int*        szFrames              // [in,out] размер массива
);

// получение кадра с определенным номером в качестве изображения
QSNAP_API QError GetFrameImage
(
  QHandle     hView,                // [in]  хэндл окна
  int         nFrame                // [in]  номер кадра
  // image..                        // [out] изображение
);

// показ определенного кадра ролика в окне просмотра/редактирования изображения
QSNAP_API QError ShowFrameImage
(
  QHandle     hFrameView,           // [in] хэндл окна показа кадров
  int         nFrame,               // [in] номер кадра
  QHandle     hImageView            // [in] хэндл окна показа изображения
);

// сохранение определенного кадра ролика в файле
QSNAP_API QError SaveFrameImage
(
  QHandle     hView,                // [in] хэндл окна показа кадров
  int         nFrame,               // [in] номер кадра
  const char* sFileName,            // [in] имя файла изображения
  ImageFormat nFormat,              // [in] формат файла
  int         nFormatParm           // [in] параметр формата (качество jpeg и проч.)
);

////////////////////////////////////////////////////////
//////    Интерактивная работа с пользователем
////////////////////////////////////////////////////////

// получение координат пользовательской точки
QSNAP_API QError GetUserPoint
(
  QHandle       hView,              // [in]  хэндл окна
  SnpPoint*     point               // [out] координаты точки
);

// получение координат пользовательского прямоугольника
QSNAP_API QError GetUserRect
(
  QHandle       hView,              // [in]  хэндл окна
  SnpRect*      rect                // [out] координаты прямоугольника
);

// получение координат пользовательского многоугольника
QSNAP_API QError GetUserPolygon
(
  QHandle       hView,              // [in]  хэндл окна
  SnpPoint*     points,             // [in,out] массив точек многоугольника
  int*          szPoints            // [in,out] размер массива
);

// получение пользовательской строки
QSNAP_API QError GetUserString
(
  QHandle       hView,              // [in]  хэндл окна
  const char*   sCaption,           // [in]  название окна диалога
  char*         pString,            // [out] пользовательская строка
  int*          szString            // [in,out] размер строки
);

// получение пользовательского числа
QSNAP_API QError GetUserLong
(
  QHandle       hView,              // [in]  хэндл окна
  const char*   sCaption,           // [in]  название окна диалога
  long*         pLong               // [out] пользовательское число
);

// получение списка строк
QSNAP_API QError GetUserStringList
(
  QHandle       hView,              // [in]  хэндл окна
  const char*   sCaption,           // [in]  название окна диалога
  char**        pStrings,           // [out] массив строк
  int*          szCount             // [in,out] размер массива
);

// получение списка ключ-значение
QSNAP_API QError GetUserStringMap
(
  QHandle       hView,              // [in]  хэндл окна
  const char*   sCaption,           // [in]  название окна диалога
  const char**  psKeys,             // [in]  массив ключей
  char**        psStrings,          // [out] массив значений, соотв. ключам
  int*          szCount             // [in,out] размер массива
);

////////////////////////////////////////////////////////
//////    Пользовательские события
////////////////////////////////////////////////////////

// регистрация пользовательского события
QSNAP_API QHandle RegisterEvent     // [ret] хэндл события
(
  QHandle       hInstance,          // [in]  хэндл экземпляра snap
  const char*   sId                 // [in]  идентификатор события
);

// получение идентификатора события
QSNAP_API const char* GetEventId    // [ret] идентификатор события
(
  QHandle       hEvent              // [in]  хэндл события snap
);

// получение идентификатора события
QSNAP_API QHandle GetEvent          // [ret] хэндл события
(
  QHandle       hInstance,          // [in]  хэндл экземпляра snap
  const char*   sId                 // [in]  идентификатор события
);

// вызов события по идентификатору
QSNAP_API QHandle FireEvent
(
  QHandle       hEvent,             // [in]  хэндл экземпляра snap
  const char*   sParm,              // [in]  основной параметр
  void*         sParmEx             // [in]  дополнительный параметр
);

// вызов события по идентификатору
QSNAP_API QHandle FireEventId
(
  QHandle       hInstance,          // [in]  хэндл экземпляра snap
  const char*   sId,                // [in]  идентификатор события
  const char*   sParm,              // [in]  основной параметр
  void*         sParmEx             // [in]  дополнительный параметр
);

// ожидание действия со стороны пользователя
// -- Пока реализовано только ожидание нажатия на клавишу и кнопки мыши
// -- При этом возвращается код клавиши либо кнопка мыши
// -- Фильтры реализованы только на уровне клавиатура/мышь и кода кнопки клавиатуры
QSNAP_API QError WaitUserInput
(
  QHandle       hView,              // [in]  хэндл окна
  WaitUserFlags waitFlags,          // [in]  тип события (mouse, keyboard)
  WaitOptions*  pWaitOptions,       // [in]  параметры ожидания (временной интервал и проч.)
  WaitResults*  pWaitResults        // [out] если указатель не нулевой, в WaitResults прописывается детальная информация о событии
);

// вывод сообщения в отдельном окне
QSNAP_API int MessageBox
(
  QHandle       hView,              // [in] хэндл окна
  const char*   sCaption,           // [in] название окна диалога
  const char*   sMessage,           // [in] текстовое сообщение
  MbType        type                // [in] тип сообщения (OK, YES_NO, и т.д.)
);

////////////////////////////////////////////////////////
//////    Работа с панелью управления
////////////////////////////////////////////////////////

// добавление элемента управления
QSNAP_API QHandle AddControl         // [ret] хэндл элемента управления
(
  QHandle         hView,            // [in]  хэндл панели управления
  QSnp::CtrlInfo* pControlInfo      // [in]  свойства элемента управления
);

// удаление вершины
QSNAP_API QError RemoveControl
(
  QHandle         hControl          // [in]  хэндл элемента управления
);

////////////////////////////////////////////////////////
//////    Пользовательские виджеты
////////////////////////////////////////////////////////

// загрузка внешней библиотеки с виджетом
QSNAP_API QHandle LoadExternalWidgetDll // [ret] хэндл загруженной библиотеки
(
  const char* dllname                 // [in]  имя внешней библиотеки для загрузки, относительно текущей директории
);

// создание виджета
QSNAP_API QHandle CreateCustomWidget  // [ret] хэндл созданного виджета
(
  QHandle hInstance                   // [in] хэндл загруженной библиотеки
);

// передача данных для отображения в виджет
QSNAP_API bool GiveDataToWidget       // [ret] успешность операции
(
  QHandle hDll,                       // [in] хэндл загруженной библиотеки
  QHandle pWidget,                    // [in] хэндл созданного виджета
  QHandle pFrame                      // [in] указатель на данные для отображения
);

// закрытие виджета
QSNAP_API QError CloseWidget
(
  QHandle hDll,                       // [in] хэндл загруженной библиотеки
  QHandle pWidget                     // [in] хэндл созданного виджета
);

};  // extern "C" {
};  // namespace QSnp 

#endif // QSNAP_H
