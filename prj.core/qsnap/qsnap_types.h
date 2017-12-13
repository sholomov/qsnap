////////////////////////////////////////////////////////////////////////
/**
 * @file   qsnap_types.h
 * @brief  types definition for QSnap library.
 */

#pragma once

#ifndef QSNAP_TYPES_H
#define QSNAP_TYPES_H

namespace QSnp {

#ifdef __GCC__
  #pragma GCC diagnostic ignored "-Weffc++"
#endif // __GCC__

///////////////////////////////////////////////////////////////////////
////////////  flags definitions

typedef int     FontFlags;
#define FF_BOLD           0x00000001    ///<  жирный
#define FF_ITALIC         0x00000002    ///<  курсив
#define FF_UNDERLINED     0x00000004    ///<  подчеркнутый

typedef int     ImageFlags;
#define IF_HSTRETCH       0x00000001    ///<  растянутое по горизонтали
#define IF_VSTRETCH       0x00000002    ///<  растянутое по вертикали
#define IF_STRETCH        0x00000003    ///<  растянутое в обоих направлениях
#define IF_ORIGINAL       0x00000004    ///<  без растяжения
#define IF_CENTRED        0x00000008    ///<  центрировано
#define IF_DONT_REPAINT   0x00000100    ///<  не перерисовывать окно

typedef int     FrameFlags;
#define FF_HSTRETCH       0x00000001    ///<  растянутое по горизонтали
#define FF_VSTRETCH       0x00000002    ///<  растянутое по вертикали
#define FF_STRETCH        0x00000003    ///<  растянутое в обоих направлениях
#define FF_ORIGINAL       0x00000004    ///<  без растяжения
#define FF_CENTRED        0x00000008    ///<  центрировано
#define FF_LAYOUT_FILM    0x00000010    ///<  layout кадров: следуют один за другим
#define FF_LAYOUT_TILE    0x00000020    ///<  ...

typedef int     WaitUserFlags;
#define WF_KEYBOARD       0x00000001    ///<  нажатие клавиши
#define WF_MOUSE          0x00000002    ///<  нажатие кнопки мыши
#define WF_TIMER          0x00000004    ///<  таймер
#define WF_USER           0x00000008    ///<  зарегистрированное пользователем
#define WF_ANY            0x0000ffff    ///<  любое из событий

typedef int     LineType;
#define LT_LINE           0x00000001    ///<  обычная линия
#define LT_ARROW          0x00000002    ///<  однонаправленная стрелка
#define LT_DARROW         0x00000003    ///<  двунаправленная стрелка

typedef int     PolygonType;
#define PT_DEFAULT        0x00000000    ///<  умолчательный
#define PT_POLYGON        0x00000000    ///<  замкнутый многоугольник
#define PT_LINES          0x00000001    ///<  ломаная (незамкнутая)

typedef int     MbType;
#define MG_OK             0x00000001    ///<  информирование
#define MG_YESNO          0x00000002    ///<  да/нет
#define MG_WARN           0x00000003    ///<  предупреждение

typedef int     ImageFormat;
#define IF_JPEG           0x00000001
#define IF_TIFF           0x00000002
#define IF_PNG            0x00000003

typedef int     SnpKey;
#define WF_VKEY_SHIFT     16
#define WF_VKEY_CTRL      17
#define WF_VKEY_ALT       18
#define WF_VKEY_ENTER     13
#define WF_VKEY_ESCAPE    27
#define WF_VKEY_SPACE     32

#define QS_STR_BUFF       256           ///<  размер буфера для типовой строки 
#define QS_MAX_EVENTS     16            ///<  максимальное количество событий для ожидания
#define UP_DEFAULT_TIME   20            ///<  умолчательное время обработки пользовательских событий msec

///////////////////////////////////////////////////////////////////////
////////////  type and enum definitions

typedef unsigned long long Handle; // все хэндлы библиотеки
typedef unsigned long long Error;  // код ошибки

typedef struct
{
  int x;
  int y;
  int width;
  int height;
} SnpRect;

typedef struct
{
  int x;
  int y;
} SnpPoint;

/**
 * @brief   View window type.
 * @details 
 */
typedef enum
{
  VT_UNDEFINED,             ///<  тип не определен
  VT_SNAPTREE_VIEW,         ///<  управляющее дерево
  VT_TEXT_VIEW,             ///<  окно лога
  VT_IMAGE_VIEW,            ///<  окно просмотра изображений
  VT_SYNC_IMAGE_VIEW,       ///<  окно синхронизованного просмотра 2 изображений
  VT_RASTER_VIEW,           ///<  окно просмотра растра
  VT_IMAGE_EDIT,            ///<  окно редактора изображения
  VT_TOOLBAR,               ///<  панель управления
  VT_LIST_VIEW,             ///<  окно просмотра списка
} ViewType;


/**
 * @brief   Orientation of SyncView window.
 * @details 
 */
typedef enum
{
  SVO_HORIZONTAL = 0,         ///<  окна расположены горизонтально, сплиттер вертикальный
  SVO_VERTICAL = 1            ///<  окна расположены вертикально, сплиттер горизонтальный
} SyncViewOrientation;

/**
 * @brief   Layout types for SyncView window.
 * @details 
 */
typedef enum
{
  SVL_DEFAULT = 0,            ///<  умолчательный layout - 1 сплиттер
  SVL_SPLIT_NONE = 1,         ///<  без разделителя
  SVL_SPLIT_ORIENT = 0,       ///<  1 сплиттер, ориентация задается в SyncViewOrientation
  SVL_SPLIT_NH = 2,           ///<  2 сплиттера, 1-й по ориентации, 2-й вертикальный в 1 окне
  SVL_SPLIT_NV = 3,           ///<  2 сплиттера, 1-й по ориентации, 2-й вертикальный во 2 окне
  SVL_SPLIT_HN = 4,           ///<  2 сплиттера, 1-й по ориентации, 2-й горизонтальный в 1 окне
  SVL_SPLIT_VN = 5,           ///<  2 сплиттера, 1-й по ориентации, 2-й вертикальный в 1 окне
  SVL_SPLIT_HH = 6,           ///<  3 сплиттера, 1-й по ориентации, 2-й горизонтальный в 1 окне, 3-й горизонтальный во 2 окне 
  SVL_SPLIT_HV = 7,           ///<  3 сплиттера, 1-й по ориентации, 2-й горизонтальный в 1 окне, 3-й вертикальный во 2 окне
  SVL_SPLIT_VH = 8,           ///<  3 сплиттера, 1-й по ориентации, 2-й вертикальный в 1 окне, 3-й горизонтальный во 2 окне
  SVL_SPLIT_VV = 9            ///<  3 сплиттера, 1-й по ориентации, 2-й вертикальный в 1 окне, 3-й вертикальный во 2 окне
} SyncViewLayoutType;

/**
 * @brief   ..............
 * @details ..............
 *          ..............
 */
typedef enum
{
  SN_CHECK,                 ///<  обычная вершина, с возможностью выделения
  SN_PRM_KEYVAL,            ///<  вершина со списком параметров ключ/значение
  SN_PRM_DIALOG             ///<  вершина с параметрами, визуализируемыми в виде диалога
} SnpNodeType;

typedef enum
{
  EVT_NONE=0,               ///<  тип не определен
  EVT_MOUSE,                ///<  шелчок мыши
  EVT_KEYBOARD,             ///<  нажатие клавиши
  EVT_TIMER,                ///<  таймер
  EVT_USER                  ///<  пользователь
} SnpEventType;

typedef enum
{
  CTRL_DEFAULT=0,           ///<  умолчательный
  CTRL_BUTTON=0,            ///<  кнопка
  CTRL_CHKBUTTON            ///<  кнопка с фиксацией
} SnpCtrlType;

/**
 * @brief   Initialization parameters.
 * @details 
 */
typedef struct
{
  char* szReserved;
} InitParams;

// Цвет в виде RGB
typedef int     SnpColor;

/**
 * @brief   Initialization parameters.
 * @details 
 */
typedef struct
{
  // свойства вершины
  char szId[QS_STR_BUFF];           ///<  текстовый идентификатор
  char szDescription[QS_STR_BUFF];  ///<  текстовое описание
  SnpNodeType type;                 ///<  тип вершины
  SnpColor    color;                ///<  цвет вершины
  
  // состояние вершины
  bool        bChecked;             ///<  выделена или нет
  bool        bExpanded;            ///<  раскрыта или нет
  char* szReserved;

} NodeInfo;

// Свойства свойства вершины
typedef struct
{
  char  sKey[QS_STR_BUFF];          ///<  название свойства
  char  sValue[QS_STR_BUFF];        ///<  значение свойства
} NodePropertyInfo;

// Аналог описания шрифта в Windows
typedef struct
{
  int   cHeight;
  int   cWidth;
  int   cEscapement;
  int   cOrientation;
  int   cWeight;
  bool  bItalic;
  bool  bUnderline;
  bool  bStrikeOut;
  long  iCharSet;
  long  iOutPrecision;
  long  iClipPrecision;
  long  iQuality;
  long  iPitchAndFamily;
  const char* szFaceName;
} SnpFont;

// Общие свойства окна просмотра
typedef struct
{
  SnpRect     rc;                  ///<  прямоугольник окна
  const char* szTitle;             ///<  текст заголовка
  bool        bVisible;            ///<  видимость окна
} ViewInfo;

// Свойства окна просмотра текста
typedef struct : public ViewInfo
{
  SnpColor    colFore;              ///<  цвет шрифта умолчательный
  SnpColor    colBack;              ///<  цвет фона умолчательный
  SnpFont     font;                 ///<  шрифт умолчательный
  bool        bShowVScroll;         ///<  показ вертикального скроллера
  bool        bShowHScroll;         ///<  показ горизонтального скроллера

} TextViewInfo;

// Свойства окна дерева управления
typedef struct : public ViewInfo
{
} SnapTreeViewInfo;

// Свойства изображения
typedef struct 
{
  // ...
} ImageInfo;

// Свойства окна просмотра изображения
typedef struct : public ViewInfo
{
  const char* szFileName;           ///<  файл изображения
  ImageInfo   imageInfo;            ///<  свойства изображения
} ImageViewInfo;

// Свойства окна просмотра кадров
typedef struct : public ViewInfo
{
  const char* szFileName;           ///<  файл изображения
  int         nFrames;              ///<  количество кадров
} FrameViewInfo;

// Свойства кадра
typedef struct 
{
  // ...
} FrameInfo;

// Параметры функции ожидания действия пользователя
typedef struct
{
  long        nTimeInterval;
  long        nMouseEventFilter;    ///<  фильтр какие события мыши ожидать
#define WME_NONE          0x00000000    ///<  нет фильтра
#define WME_LBTNCLICK     0x00000001    ///<  нажатие левой клавиши
#define WME_RBTNCLICK     0x00000002    ///<  нажатие правой клавиши
#define WME_LBTNDBLCLICK  0x00000003    ///<  двойное нажатие левой клавиши
#define WME_RBTNDBLCLICK  0x00000004    ///<  двойное нажатие правой клавиши
#define WME_WHEEL         0x00000005    ///<  вращение колесика

  long        nKbdEventFilter;      ///<  фильтр какие события клавиатуры ожидать
#define WKF_NONE      0x00000000        ///<  нет фильтра
#define WKF_KEY       0x00000001        ///<  нажатие клавиши

  long        nKbdFilterData;       ///<  данные для фильтрации событий в зависимости от фильтра
                                        ///<  WKF_KEY - код клавиши

  long        nModifiers;           ///<  при каких модификаторах клавиатуры
#define WKM_ANY       0x00000000        ///<  при любых
#define WKM_CTRL      0x00000001        ///<  нажатие клавиши
#define WKM_ALT       0x00000002        ///<  нажатие левой кнопки мыши
#define WKM_SHIFT     0x00000004        ///<  нажатие правой кнопки мыши
#define WKM_NONE      0x00000008        ///<  только если не нажаты модификаторы

                                    ///<  ожидаемые пользовательские события
#define QS_MAX_EVENTS 16
  const char* sUserEventIds[QS_MAX_EVENTS];       ///<  множество ожидаемых id событий

} WaitOptions;

// Результат пользовательского ввода - код кнопки и прочее
typedef struct
{
  SnpEventType  eventType;          ///<  тип события
  QSnp::Handle  eventData;          ///<  дополнительная информация о событии
                                          ///<  EVT_MOUSE:      код события WME_***
                                          ///<  EVT_KEYBOARD:   код клавиши
                                          ///<  EVT_TIMER:      0
                                          ///<  EVT_USER:       ID события
  long          nModifiers;         ///<  расширенная информация о модификаторах клавиатуры (WKM_***)
} WaitResults;

/**
 * @brief   Initialization parameters.
 * @details 
 */
typedef struct
{
  SnpCtrlType type;                   ///<  тип элемента управления
  
  // свойства элемента
  char szId[QS_STR_BUFF];             ///<  текстовый идентификатор события
  char szCaption[QS_STR_BUFF];        ///<  название при визуализации
  char szDescription[QS_STR_BUFF];    ///<  текстовое описание
  
  // состояние элемента
  bool    bChecked;                   ///<  выделена или нет

} CtrlInfo;

}; // namespace QSnp 

///////////////////////////////////////////////////////////////////////
////////////  определение упрощенных типов без namespace QSnp

#ifndef _QTYPES_CANCEL_SIMPLE

#define QError    QSnp::Error
#define QHandle   QSnp::Handle
#define QSnapNode QSnp::Handle

#define _QTYPES_CANCEL_SIMPLE 
#endif

#define QERR_NO_ERROR                  1
#define QERR_ERROR                     0
#define QERR_NOTIMPLEMENTED           -1


#define QHANDLE_NULL            0
#define QHANDLE_INVALID         QHANDLE_NULL


#endif // QSNAP_TYPES_H
