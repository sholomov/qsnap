/**
  \file   qspx_impl.h
  \brief  QSnap proxy functions implementation
  \author Sholomov D.
  \date   05.06.2013
*/

#pragma once

#ifndef QSPX_IMPL_H
#define QSPX_IMPL_H

#pragma warning (disable:4800)
#pragma warning (disable:4551)

#ifdef __GCC__
  #pragma GCC diagnostic ignored "-Weffc++"
#endif // __GCC__

#include <cstdio>
#include <cstdarg>
#include <typeinfo>

#include <qsnap/qsnap.h>
#include <qsnap/qsnap_cast.h>
#include <qsnap/qsnap_proxy.h>

////////////////////////////////////////////////////////
//////    QSpxInstance functions implementation

QHandle QSpxLoadLibrary();
QError  QSpxFreeLibrary(QHandle hLibrary);
QHandle QSpxGetFunction(QHandle hLibrary, const char* pFuncName);

// конструктор аттачем к существующему объекту
inline QSpxInstance::QSpxInstance(
      QHandle _hInstance          // [in] хэндл view для присоединения к существующему объекту
      ) :
    
    hInstance(0),
    hLibrary(0), 
    bAttached(false),

    QW_INIT_NULL(Initialize),
    QW_INIT_NULL(Terminate),
    QW_INIT_NULL(SaveCurrentViewConfig),
    QW_INIT_NULL(LoadViewConfig),
    QW_INIT_NULL(CreateView),
    QW_INIT_NULL(DestroyView),
    QW_INIT_NULL(DestroyAllViews),
    QW_INIT_NULL(RegisterEvent),
    QW_INIT_NULL(GetEventId),
    QW_INIT_NULL(GetEvent),
    QW_INIT_NULL(FireEvent),
    QW_INIT_NULL(FireEventId),
    QW_INIT_NULL(WaitUserInput)
{
  if(_hInstance == 0)
    return;

  attach(_hInstance);
}

// деструктор
inline QSpxInstance::~QSpxInstance()
{   
  if(!bAttached) 
    terminate(); 
}

// инициализация инстанса снэпа
inline QError QSpxInstance::initialize(bool bThreadMode)
{
  if (hInstance)
    return QERR_NO_ERROR;

  // загрузка библиотеки
  if (hLibrary==QHANDLE_NULL)
    hLibrary = QSpxLoadLibrary();

  // инициализация динамически подгружаемых функций
  QW_INIT(Initialize);
  QW_INIT(Terminate);
  QW_INIT(LoadViewConfig);
  QW_INIT(SaveCurrentViewConfig);
  QW_INIT(CreateView);
  QW_INIT(DestroyView);
  QW_INIT(DestroyAllViews);
  QW_INIT(RegisterEvent);
  QW_INIT(GetEventId);
  QW_INIT(GetEvent);
  QW_INIT(FireEvent);
  QW_INIT(FireEventId);
  QW_INIT(WaitUserInput);

  // инициализация экземпляра снепа
  hInstance = QW_CALL(Initialize)(bThreadMode);

  bAttached = false;

  return hInstance != QHANDLE_NULL ? QERR_NO_ERROR : QERR_ERROR;
}

// деинициализация инстанса снэпа
inline QError QSpxInstance::terminate()
{
  if (hInstance)
  {
    QW_CALL(Terminate)(hInstance);
    hInstance = QHANDLE_NULL;
  }

  if (hLibrary)
  {
    QSpxFreeLibrary(hLibrary);
    hLibrary = QHANDLE_NULL;
  }

  return QERR_NO_ERROR;
}

inline QHandle QSpxInstance::handle()
{
  return hInstance;
}

inline QHandle QSpxInstance::library()
{
  return hLibrary;
}

// присоединение объекта-хэлпера к объекту Instance по хэндлу
inline bool QSpxInstance::attach(QHandle _hInstance)
{
  hInstance = _hInstance;

  // загрузка библиотеки
  if (hLibrary==QHANDLE_NULL)
    hLibrary = QSpxLoadLibrary();

  // инициализация динамически подгружаемых функций
  QW_INIT(Initialize);
  QW_INIT(Terminate);
  QW_INIT(LoadViewConfig);
  QW_INIT(SaveCurrentViewConfig);
  QW_INIT(CreateView);
  QW_INIT(DestroyView);
  QW_INIT(DestroyAllViews);
  QW_INIT(RegisterEvent);
  QW_INIT(GetEventId);
  QW_INIT(GetEvent);
  QW_INIT(FireEvent);
  QW_INIT(FireEventId);
  QW_INIT(WaitUserInput)

  bAttached = true;
  return true;
}

// отсоединение объекта-хэлпера от объекта Instance
inline bool QSpxInstance::detach()
{
  // обнуление указателей на функции
  pInitialize = 0;
  pTerminate = 0;
  pLoadViewConfig = 0;
  pSaveCurrentViewConfig = 0;
  pCreateView = 0;
  pDestroyView = 0;
  pDestroyAllViews = 0;
  pRegisterEvent = 0;
  pGetEventId = 0;
  pGetEvent = 0;
  pFireEvent = 0;
  pFireEventId = 0;
  pWaitUserInput = 0;

  // выгрузка библиотеки  
  QSpxFreeLibrary(hLibrary);
  hLibrary = 0;
  hInstance = 0;

  bAttached = false;

  return true;
}


inline QError QSpxInstance::saveCurrentViewConfig(
    const char* szConfigName
)
{
  if (!hInstance || !pSaveCurrentViewConfig)
    return QERR_ERROR;

  return QW_CALL(SaveCurrentViewConfig)(hInstance, szConfigName);
}

inline QError QSpxInstance::loadViewConfig(
    const char* szConfigName
)
{
  if (!hInstance || !pLoadViewConfig)
    return QERR_ERROR;

  return QW_CALL(LoadViewConfig)(hInstance, szConfigName);
}

// создание окна просмотра
inline QSpxView* QSpxInstance::createView(
    QSnp::ViewType eViewType,           // [in]  тип окна просмотра
    const char*    sId,                 // [in]  идентификатор окна
    long long      parm                 // [in]  дополнительный параметр

)
{
  if(hInstance && pCreateView)
  {
    QHandle hView = QW_CALL(CreateView)(hInstance, eViewType, sId);
    if(hView==QHANDLE_NULL)
      return QHANDLE_NULL;

    QSpxView* pView = 0;
    switch(eViewType)
    {
    case VT_SNAPTREE_VIEW:    ///<  управляющее дерево
      pView = new QSpxTreeView(hView, false);
      break;

    case VT_TEXT_VIEW:        ///<  окно лога
      pView = new QSpxTextView(hView, false);
      break;

    case VT_IMAGE_VIEW:       ///<  окно изображения
      pView = new QSpxImageView(hView, false);
      break;

    case VT_SYNC_IMAGE_VIEW:  ///<  окно синхронизованных изображения
      pView = new QSpxSyncImageView(hView, false);
      break;

    case VT_TOOLBAR:          ///<  панель управления
      pView = new QSpxToolbar(hView, false);
      break;

    case VT_LIST_VIEW:        ///<  окно списка
      pView = new QSpxListView(hView, false);
      break;

    default:
      pView = 0;
    }

    if(!pView)
      return QHANDLE_NULL;

    pView->setViewHandle(hView);
    //pView->setInstanceHandle(hInstance);

    return pView;
  }

  return QHANDLE_NULL;
}

// создание окна внешнего виджета
inline QSpxExternalWidget* QSpxInstance::createWidget(const char* dllname)
{
  if (!(hInstance))
    return QHANDLE_NULL;

  return new QSpxExternalWidget(dllname, hLibrary);
}

// загрузить состояние вершин
inline void QSpxTreeView::loadState()
{
  QW_SAFECALL(pLoadTreeViewState)(hView);
}

// сохранить состояние вершин
inline void QSpxTreeView::saveState()
{
  QW_SAFECALL(pSaveTreeViewState)(hView);
}

template<class TView> 
inline TView* QSpxInstance::createView(
  const char* sId                     // [in]  идентификатор окна
)
{
  if (typeid(TView) == typeid(QSpxTreeView))
    return static_cast<TView*>(createView(VT_SNAPTREE_VIEW, sId));
  else if (typeid(TView) == typeid(QSpxTextView))
    return static_cast<TView*>(createView(VT_TEXT_VIEW, sId));
  else if (typeid(TView) == typeid(QSpxImageView))
    return static_cast<TView*>(createView(VT_IMAGE_VIEW, sId));
  else if (typeid(TView) == typeid(QSpxSyncImageView))
    return static_cast<TView*>(createView(VT_SYNC_IMAGE_VIEW, sId));
  else if (typeid(TView) == typeid(QSpxToolbar))
    return static_cast<TView*>(createView(VT_TOOLBAR, sId));
  else
    return 0;
}

// уничтожение окна просмотра
inline QError QSpxInstance:: destroyView(
  QSpxView* pView                         // [in]  хэндл окна
)
{
  if (!pView)
    return QERR_ERROR;

  delete pView;

  return QERR_NO_ERROR;
}

inline QError QSpxInstance::destroyAllViews()
{
  return QERR_ERROR;
}

// перерисовка всех окон просмотра
inline bool QSpxInstance::updateAllViews()
{
  wait(1);
  return true;
}

// регистрация пользовательского события
inline QHandle QSpxInstance::registerEvent(     // [ret] хэндл события
  const char*   sId                 // [in]  идентификатор события
)
{
  if(hInstance == QHANDLE_NULL)
    return QERR_ERROR;

  return QW_CALL(RegisterEvent)(handle(), sId);
}

// получение идентификатора события
inline const char* QSpxInstance::getEventId(    // [ret] идентификатор события
  QHandle       hEvent              // [in]  хэндл события snap
)
{
  if(hInstance == QHANDLE_NULL)
    return QERR_ERROR;

  return QW_RETCALL(pGetEventId)(hEvent);
}

// получение идентификатора события
inline QHandle QSpxInstance::getEvent(          // [ret] хэндл события
  const char*   sId                 // [in]  идентификатор события
)
{
  if(hInstance == QHANDLE_NULL)
    return QERR_ERROR;

  return QW_CALL(GetEvent)(handle(), sId);
}

// вызов события по идентификатору
inline bool QSpxInstance::fireEvent(
  QHandle       hEvent,             // [in]  хэндл экземпляра snap
  const char*   sParm,              // [in]  основной параметр
  void*         sParmEx             // [in]  дополнительный параметр
)
{
  if(hInstance == QHANDLE_NULL)
    return QERR_ERROR;

  return QW_CALL(FireEvent)(hEvent, sParm, sParmEx);
}

// вызов события по идентификатору
inline QHandle QSpxInstance::fireEvent(
  const char*   sId,                // [in]  идентификатор события
  const char*   sParm,              // [in]  основной параметр
  void*         sParmEx             // [in]  дополнительный параметр
)
{
  if(hInstance == QHANDLE_NULL)
    return QERR_ERROR;

  return QW_CALL(FireEventId)(handle(), sId, sParm, sParmEx);
}

// ожидание события пользователя
inline QError QSpxInstance::waitUserInput(
    WaitUserFlags waitFlags,          // [in] тип события (mouse, keyboard)
    WaitOptions*  pWaitFlags,         // [in] параметры ожидания (временной интервал и проч.)
    WaitResults*  pWaitResults        // [out] если указатель не нулевой, в WaitResults прописывается детальная информация о событии
)
{
  return waitUserInput(handle(), waitFlags, pWaitFlags, pWaitResults);
}

// ожидание действия пользователя
inline QHandle QSpxInstance::wait(    // [ret] клавиша или id события "продолжить"
    long nTimeInterval                // [in] таймаут ожидания в мсек
)
{
  WaitOptions waitOptions = {nTimeInterval};
  WaitResults waitResults = {};
  waitUserInput(WF_KEYBOARD | WF_TIMER , &waitOptions, &waitResults);
  return waitResults.eventData;
}

inline QError QSpxInstance::waitUserInput(
    QHandle hView,                    // [in] хэндл окна, может быть NULL, тогда события
    WaitUserFlags waitFlags,          // [in] тип события (mouse, keyboard)
    WaitOptions*  pWaitFlags,         // [in] параметры ожидания (временной интервал и проч.)
    WaitResults*  pWaitResults        // [out] если указатель не нулевой, в WaitResults прописывается детальная информация о событии
)
{
  if(hInstance == QHANDLE_NULL)
    return QERR_ERROR;

  return QW_CALL(WaitUserInput)(handle(), waitFlags, pWaitFlags, pWaitResults);
}


///////////////////////////////////////////////////////////////////////
////  QSnwView class

inline QSpxView::QSpxView(
    QHandle _hView,               // [in] хэндл view для присоединения к существующему объекту
    bool bAttach                  // [in] true при присоединении окна, при этом отсутствует его разрушение в деструкторе
)
{
  hView = QHANDLE_NULL;
//  pInstance = _pInstance;

  hLibrary = QSpxLoadLibrary(); 

  // инициализация динамически подгружаемых функций
  QW_INIT(DestroyView);
  QW_INIT(ShowView);
  QW_INIT(UpdateView);
  QW_INIT(GetViewInfo);
  QW_INIT(SetViewInfo);
  QW_INIT(GetInstance);

  bAttached = false;

  // присоединение к существующему объекту view
  if(bAttach && _hView != QHANDLE_NULL)
    attach(_hView);
}

// деструктор
inline QSpxView::~QSpxView()
{
  if(bAttached)
    detach();
  else
    destroy();

  if(hLibrary)
  {
    QSpxFreeLibrary(hLibrary);
    hLibrary = QHANDLE_NULL;
  }
}

// уничтожение окона просмотра
inline QError QSpxView::destroy()
{
  if(hView == QHANDLE_NULL)
    return QERR_ERROR;

  if(bAttached)
    return QERR_ERROR;

  QError err = QW_CALL(DestroyView)(hView);

  if(err!=QERR_NO_ERROR)
    return err;

  hView = QHANDLE_NULL;
  return QERR_NO_ERROR;
}

// получение хэндла окна просмотра
inline QHandle QSpxView::handle()
{
  return hView;
}

// присоединение объекта-хэлпера к объекта View по хэндлу
inline bool QSpxView::attach(QHandle _hView)
{
  hView = _hView;
  bAttached = true;
  return true;
}

// отсоединение объекта-хэлпера от объекта View
inline bool QSpxView::detach()
{
  hView = 0;
  bAttached = false;
  return true;
}

// показ/скрытие окна просмотра
inline QError QSpxView::show(bool bShow)
{
  if(hView==QHANDLE_NULL)
    return QERR_ERROR;

  return QW_CALL(ShowView)(hView, bShow);
}

// перерисовка окна
inline QError QSpxView::update(QSnp::SnpRect* pRect)
{
  if(hView==QHANDLE_NULL)
    return QERR_ERROR;

  return QW_CALL(UpdateView)(hView, pRect);
}

inline bool QSpxView::isVisible()
{
  if(hView==QHANDLE_NULL)
    return QERR_ERROR;

  QSnp::ViewInfo viewInfo={0};
  if(QW_CALL(GetViewInfo)(hView, &viewInfo)==QERR_ERROR)
    return false;
  return viewInfo.bVisible;
}

// прямоугольник окна
inline QSnp::SnpRect QSpxView::rect()
{
  QSnp::SnpRect rcNull = {0};
  if(hView==QHANDLE_NULL)
    return rcNull;

  QSnp::ViewInfo viewInfo={0};
  if(QW_CALL(GetViewInfo)(hView, &viewInfo)==QERR_ERROR)
    return rcNull;
  return viewInfo.rc;
}

inline void QSpxView::setRect(const QSnp::SnpRect& _rc)
{
  if(hView==QHANDLE_NULL)
    return;

  QSnp::ViewInfo viewInfo={0};
  if(QW_CALL(GetViewInfo)(hView, &viewInfo)==QERR_ERROR)
    return;

  viewInfo.rc = _rc;
  QW_CALL(SetViewInfo)(hView, &viewInfo);

}

// текст заголовка
inline const char* QSpxView::title()
{
  if(hView==QHANDLE_NULL)
    return NULL;

  QSnp::ViewInfo viewInfo={0};
  if(QW_CALL(GetViewInfo)(hView, &viewInfo)==QERR_ERROR)
    return NULL;

  return viewInfo.szTitle;
}

// текст заголовка
inline void QSpxView::setTitle(const char* _title)
{
  if(hView==QHANDLE_NULL)
    return;

  QSnp::ViewInfo viewInfo={0};
  if(QW_CALL(GetViewInfo)(hView, &viewInfo)==QERR_ERROR)
    return;

  viewInfo.szTitle = _title;
  QW_CALL(SetViewInfo)(hView, &viewInfo);
}

// ожидание события пользователя
inline QError QSpxView::waitUserInput(
      WaitUserFlags waitFlags,          // [in] тип события (mouse, keyboard)
      WaitOptions*  pWaitFlags,         // [in] параметры ожидания (временной интервал и проч.)
      WaitResults*  pWaitResults        // [out] если указатель не нулевой, в WaitResults прописывается детальная информация о событии
  )
{

  QHandle hInstance = QW_CALL(GetInstance)(hView);
  QSpxInstance instance(hInstance);

  return instance.waitUserInput(hView, waitFlags, pWaitFlags, pWaitResults);
}

///////////////////////////////////////////////////////////////////////
////  QSpxImageView class

inline QSpxImageView::QSpxImageView(QHandle hView, bool bAttach) : QSpxView(hView, bAttach)
{
  // инициализация динамически подгружаемых функций
  QW_INIT(SetImage);
  QW_INIT(SetMatImage);

#ifdef __MINIMG__
  QW_INIT(SetMinImage);
#endif
  //QW_INIT(GetImageViewInfo);
  //QW_INIT(SetImageViewInfo);
  //QW_INIT(ImageScaleToRect);
  QW_INIT(SetScaleFactor);
  QW_INIT(DrawPoint);
  QW_INIT(DrawLine);
  QW_INIT(DrawRect);
  //QW_INIT(DrawPolygon);
  QW_INIT(DrawEllipse);
  //QW_INIT(DrawArc);
  QW_INIT(DrawText);
  QW_INIT(GetUserPoint);
  QW_INIT(GetUserRect);
  QW_INIT(ShowFigure);
  QW_INIT(ClearFigures);
}

inline QSpxImageView::~QSpxImageView()
{
}

inline QError QSpxImageView::setImage
(
  const char*       sFileName,            // [in]  файл изображения
  QSnp::ImageFlags  flagsShow             // [in]  флаги показа изображения
)
{
  return QW_CALL(SetImage)(hView, sFileName, flagsShow);
}

inline QError QSpxImageView::setImage
(
  const cv::Mat&    image,                // [in]  cv::Mat изображение
  QSnp::ImageFlags  flagsShow             // [in]  флаги показа изображения
)
{
  if(image.channels()==1)
  {
    cv::Mat channels[3] = {image, image, image}; cv::Mat3b grayImage;
    cv::merge(channels, 3, grayImage);
    return QW_CALL(SetMatImage)(hView, &grayImage, flagsShow);
  }
  return QW_CALL(SetMatImage)(hView, &image, flagsShow);
}

#ifdef __MINIMG__

inline QError QSpxImageView::setImage
(
  const MinImg*     minImage,             // [in]  изображение
  QSnp::ImageFlags  flagsShow             // [in]  флаги показа изображения
)
{
  return QW_CALL(SetMinImage)(hView, minImage, flagsShow);
}

#endif

// установка коэффициента масштабирования фигур в координатах изображения
inline QError QSpxImageView::setScaleFactor
(
  double        scale,              // [in] координата по горизонтали
  const char*   idGroup             // [in] идентификатор группы фигур (not implemented)
)
{
  return QW_CALL(SetScaleFactor)(hView, scale, idGroup);
}

// отрисовка точки
inline QSpxFigure* QSpxImageView::drawPoint         // [ret] хэндл фигуры
(
  int           x,                  // [in] координата по горизонтали
  int           y,                  // [in] координата по вертикали
  int           ptWidth,            // [in] толщина точки (диаметр)
  SnpColor      color,              // [in] цвет
  const char*   idGroup             // [in] идентификатор группы фигур (для совместной работы)
)
{
  QW_CALL(DrawPoint)(hView, x, y, ptWidth, color, idGroup);
  return NULL;
}

// отрисовка линии
inline QSpxFigure* QSpxImageView::drawLine          // [ret] хэндл фигуры
(
  int           xFrom,              // [in] координата начала по горизонтали
  int           yFrom,              // [in] координата начала по вертикали
  int           xTo,                // [in] координата конца по горизонтали
  int           yTo,                // [in] координата конца по вертикали
  int           lineWidth,          // [in] толщина линии
  SnpColor      color,              // [in] цвет
  LineType      type,               // [in] тип линии (линия, стрелка, пунктир и т.д.)
  const char*   idGroup,            // [in] идентификатор группы фигур
  bool          bRepaint            // [in] отрисовка окна
)
{
  QW_CALL(DrawLine)(hView, xFrom, yFrom, xTo, yTo, lineWidth, color, type, idGroup);
  if(bRepaint)
  {
    SnpRect rect = {xFrom, yFrom, xTo-xFrom, yTo-yFrom};
    this->update(&rect);
  }
  return NULL;
}

// отрисовка прямоугольника
inline QSpxFigure* QSpxImageView::drawRect          // [ret] хэндл фигуры
(
  SnpRect       rect,               // [in] координаты прямоугольника
  int           lineWidth,          // [in] толщина линии
  SnpColor      color,              // [in] цвет
  LineType      type,               // [in] тип линии (линия, пунктир и т.д.)
  const char*   idGroup,            // [in] идентификатор группы фигур
  bool          bRepaint            // [in] отрисовка окна
)
{
  QW_CALL(DrawRect)(hView, &rect, lineWidth, color, type, idGroup);
  if(bRepaint)
    this->update(&rect);
  return NULL;
}

// отрисовка многоугольника, ломанной
inline QSpxFigure* QSpxImageView::drawPolygon       // [ret] хэндл фигуры
(
  SnpPoint*     points,             // [in] массив точек многоугольника
  int*          szPoints,           // [in,out] размер массива точек
  int           lineWidth,          // [in] толщина линии
  SnpColor      color,              // [in] цвет
  PolygonType   type,               // [in] тип многоугольника (ломаная, замкнутый и т.д.)
  const char*   idGroup,            // [in] идентификатор группы фигур
  bool          bRepaint            // [in] отрисовка окна
)
{
  return NULL;
}

// отрисовка эллипса
inline QSpxFigure* QSpxImageView::drawEllipse       // [ret] хэндл фигуры
(
  SnpRect       rect,               // [in] координаты охватывающего прямоугольника
  int           lineWidth,          // [in] толщина линии
  SnpColor      color,              // [in] цвет
  LineType      type,               // [in] тип линии (линия, пунктир и т.д.)
  const char*   idGroup,            // [in] идентификатор группы фигур
  bool          bRepaint            // [in] отрисовка окна
)
{
  QW_CALL(DrawEllipse)(hView, &rect, lineWidth, color, type, idGroup);
  if(bRepaint)
    this->update(&rect);
  return NULL;
}

// отрисовка дуги
inline QSpxFigure* QSpxImageView::drawArc           // [ret] хэндл фигуры
(
  int           lineWidth,          // [in] толщина линии
  SnpColor      color,              // [in] цвет
  LineType      type,               // [in] тип линии (линия, пунктир и т.д.)
  const char*   idGroup,            // [in] идентификатор группы фигур
  bool          bRepaint            // [in] отрисовка окна
)
{
  return NULL;
}

// отрисовка текста
inline QSpxFigure* QSpxImageView::drawText          // [ret] хэндл фигуры
(
  SnpRect       rect,               // [in] координаты охватывающего прямоугольника
  const char*   textValue,          // [in] текстовое значение
  double        fontSize,           // [in] кегль шрифта
  SnpColor      fontColor,          // [in] цвет шрифта
  const char*   fontType,           // [in] тип шрифта
  const char*   idGroup,            // [in] идентификатор группы фигур
  bool          bRepaint            // [in] отрисовка окна
)
{
  QW_CALL(DrawText)(hView, &rect, textValue, fontSize, fontColor, fontType, idGroup);
  if(bRepaint)
    this->update(&rect);
  return NULL;
}

// выставлена ли пользовательская точка
inline bool QSpxImageView::userPointSet()
{
  QSnp::SnpPoint ptNull={0};
  QSnp::SnpPoint ptUser={0};
  QW_CALL(GetUserPoint)(hView, &ptUser);
  return cvPoint_cast(ptUser)!=cvPoint_cast(ptNull);
}

// получение координат пользовательской точки
inline QSnp::SnpPoint QSpxImageView::getUserPoint()
{
  QSnp::SnpPoint ptUser={0};
  QW_CALL(GetUserPoint)(hView, &ptUser);
  return ptUser;
}

// содержит ли прямоугольник пользовательскую точку
inline bool QSpxImageView::containUserPoint(QSnp::SnpRect rc)
{
  QSnp::SnpPoint ptUser=getUserPoint();
  return cvPoint_cast(ptUser).inside(QSnp::cvRect_cast(rc));
}

// содержится ли прямоугольник полностью внутри пользовательского прямоугольника
inline bool QSpxImageView::insideUserRect(QSnp::SnpRect _rc)
{
  cv::Rect rcUser=cvRect_cast(getUserRect());
  cv::Rect rc=cvRect_cast(_rc);
  return (rcUser & rc) == rc;
}

// выставлен ли пользовательский прямоугольник
inline bool QSpxImageView::userRectSet()
{
  QSnp::SnpRect rcNull={0}; 
  QSnp::SnpRect rcUser={0};
  QW_CALL(GetUserRect)(hView, &rcUser);
  return cvRect_cast(rcUser)!=cvRect_cast(rcNull);
}

// получение координат пользовательского прямоугольника
inline QSnp::SnpRect QSpxImageView::getUserRect()
{
  QSnp::SnpRect rcUser={0};
  QW_CALL(GetUserRect)(hView, &rcUser);
  return rcUser;
}

/*
// функция показа/скрытия фигуры
QError showFigure
(
  QHandle       hFigure,            // [in] хэндл фигуры
  bool          bShow               // [in] показать/скрыть
);
*/

// функция удаления всех фигур в данном окне
inline QError QSpxImageView::clearFigures()
{
  return QW_CALL(ClearFigures)(hView);
}

///////////////////////////////////////////////////////////////////////
////  QSpxSyncImageView class

// конструктор
inline QSpxSyncImageView::QSpxSyncImageView(QHandle hView, bool bAttach) : QSpxImageView(hView, bAttach)
{
  // инициализация динамически подгружаемых функций
  QW_INIT(SetSubImage);
  QW_INIT(SetSubMatImage);

#ifdef __MINIMG__
  QW_INIT(SetSubMinImage);
#endif

}

inline QSpxSyncImageView::~QSpxSyncImageView()
{
  return;
}

// установка изображения
inline QError QSpxSyncImageView::setSubImage
(
  const char* sFileName,              // [in]  файл изображения
  int nSubView,                       // [in]  номер внутреннего окна (для SynqImageView и проч.)
  QSnp::ImageFlags  flagsShow         // [in]  флаги показа изображения
)
{
  return QW_CALL(SetSubImage)(hView, sFileName, nSubView, flagsShow);
}

// установка изображения
inline QError QSpxSyncImageView::setSubImage
(
  const cv::Mat& image,               // [in]  файл изображения
  int nSubView,                       // [in]  номер внутреннего окна (для SynqImageView и проч.)
  QSnp::ImageFlags  flagsShow         // [in]  флаги показа изображения
)
{
  if(image.channels()==1)
  {
    cv::Mat channels[3] = {image, image, image}; cv::Mat3b grayImage;
    cv::merge(channels, 3, grayImage);
    return QW_CALL(SetSubMatImage)(hView, &grayImage, nSubView, flagsShow);
  }
  return QW_CALL(SetSubMatImage)(hView, &image, nSubView, flagsShow);
}

#ifdef __MINIMG__

// установка изображения
inline QError QSpxSyncImageView::setSubImage
(
  const MinImg* pMinImage,            // [in]  изображение
  int nSubView,                       // [in]  номер внутреннего окна (для SynqImageView и проч.)
  QSnp::ImageFlags  flagsShow         // [in]  флаги показа изображения
)
{
  return QW_CALL(SetSubMinImage)(hView, pMinImage, nSubView, flagsShow);
}

#endif

////////////////////////////////////////////////////////
//////    Platform dependent service functions

#include <string.h>

#ifdef _MSC_VER // OS Windows implementation

#pragma warning ( disable:4005 )
#define NOMINMAX
#include <windows.h>

#ifdef UNICODE
  #define _strcat wcscat
  #define _strlen wcslen
#else
  #define _strcat strcat
  #define _strlen strlen
#endif

inline QHandle QSpxLoadLibrary()
{
  using namespace std;
  TCHAR szFileName[260]={0};
  ::GetModuleFileName(0, szFileName,sizeof(szFileName)-1);
  bool bSlashFound=false;
  int len = (int)_strlen(szFileName);
  // поиск последнего слэша
  for(int i = len-1; i>=0; i--)
  {
    if(szFileName[i]==TCHAR('\\') || szFileName[i]==TCHAR('/'))
    {
      szFileName[i] = 0;
      bSlashFound = true;
      break;
    }
  }

  if(!bSlashFound)
    return QHANDLE_NULL;

  TCHAR dllName[] = __TEXT("\\qsnap.dll");
  _strcat(szFileName,dllName);
  HANDLE hSnap = ::LoadLibrary(szFileName);

  return (QHandle)hSnap;
}

inline QHandle QSpxLoadSpecialLibrary(const char* pszDllName)
{
  using namespace std;
  TCHAR szFileName[260]={0};
  ::GetModuleFileName(0, szFileName,sizeof(szFileName)-1);
  bool bSlashFound=false;
  int len = (int)_strlen(szFileName);
  // поиск последнего слэша
  for(int i = len-1; i>=0; i--)
  {
    if(szFileName[i]==TCHAR('\\') || szFileName[i]==TCHAR('/'))
    {
      szFileName[i] = 0;
      bSlashFound = true;
      break;
    }
  }

  if(!bSlashFound)
    return QHANDLE_NULL;

  TCHAR dllFullName[] = __TEXT("\\");

  _strcat(szFileName, dllFullName);
  _strcat(szFileName, pszDllName);
  _strcat(szFileName, ".dll");

  HANDLE hSnap = ::LoadLibrary(szFileName);

  return (QHandle)hSnap;
}

// выгрузка библиотеки снепа
inline QError QSpxFreeLibrary
(
  QHandle       hLibrary            // [in] хэндл библиотеки
)
{
  if(hLibrary==QHANDLE_NULL)
    return QERR_NO_ERROR;
  if(!::FreeLibrary((HMODULE)hLibrary))
    return QERR_ERROR;
  hLibrary = NULL;
  return QERR_NO_ERROR;
}

// получение адреса функции
inline QHandle QSpxGetFunction
(
  QHandle       hLibrary,           // [in] хэндл библиотеки
  const char*   pFuncName           // [in] имя функции
)
{
  QHandle hFunc = (QHandle)::GetProcAddress((HMODULE)hLibrary, pFuncName);
  return hFunc;
}

#else // Linux or Mac OS X

#include <dlfcn.h>

inline QHandle QSpxLoadLibrary()
{
  #ifdef __APPLE__
    void *handle = dlopen("libqsnap.dylib", RTLD_LAZY);
  #else
    void *handle = dlopen("libqsnap.so", RTLD_LAZY);
  #endif

  return (QHandle)handle;
}

inline QHandle QSpxLoadSpecialLibrary(const char* pszDllName)
{
  std::string dllName(pszDllName);
  dllName = "lib" + dllName;

#ifdef __APPLE__
  dllName += ".dylib";
#else
  dllName += ".so";  
#endif

  void *handle = dlopen(dllName.c_str(), RTLD_LAZY);

  return (QHandle)handle;
}

// выгрузка библиотеки снепа
inline QError QSpxFreeLibrary
(
  QHandle       hLibrary            // [in] хэндл библиотеки
)
{
  if (hLibrary == QHANDLE_NULL)
    return QERR_NO_ERROR;
  if (dlclose((void *)hLibrary) != 0)
    return QERR_ERROR;
  return QERR_NO_ERROR;
}

// получение адреса функции
inline QHandle QSpxGetFunction
(
  QHandle       hLibrary,           // [in] хэндл библиотеки
  const char*   pFuncName           // [in] имя функции
)
{
  void *symbolAddress = dlsym((void *)hLibrary, pFuncName);
  return (QHandle)symbolAddress;
}

#endif // Platform dependent service functions

///////////////////////////////////////////////////////////////////////
////  QSpxTextView class

inline QSpxTextView::QSpxTextView(QHandle hView, bool bAttach) : QSpxView(hView, bAttach)
{
  // инициализация динамически подгружаемых функций
  QW_INIT(Log);
  QW_INIT(LogLn);
  QW_INIT(LogEx);
  QW_INIT(ClearTextView);
}

inline QSpxTextView::~QSpxTextView()
{
}

// вывод текстового сообщения
inline QError QSpxTextView::log
(
  const char* sText,                 // [in]  текстовый шаблон
  ...                                // [in]  параметры шаблона
)
{
  char strbuf[4096]; strbuf[0]=0;
  va_list list;
  va_start(list, sText );
  vsprintf( strbuf, sText, list );
  va_end( list );

  return QW_CALL(Log)(hView, strbuf);
}

// вывод текстового сообщения
inline QError QSpxTextView::logLn
(
  const char* sText,                 // [in]  текстовый шаблон
  ...                                // [in]  параметры шаблона
)
{
  char strbuf[4096]; strbuf[0]=0;
  va_list list;
  va_start(list, sText );
  vsprintf( strbuf, sText, list );
  va_end( list );

  return QW_CALL(LogLn)(hView, strbuf);
}


// вывод расширенного текстового сообщения
inline QError QSpxTextView::logEx
(
  const char* sText,                 // [in]  текстовый шаблон
  SnpColor    color,                 // [in]  цвет текста
  FontFlags   flags                  // [in]  параметры шрифта
)
{
  return QW_CALL(LogEx)(hView, sText, color, flags);
}

// очистка текста
inline QError QSpxTextView::clear()
{
  return QW_CALL(ClearTextView)(hView);
}


///////////////////////////////////////////////////////////////////////
////  QSpxTreeView class

inline QSpxListView::QSpxListView(QHandle hView, bool bAttach)  : QSpxView(hView, bAttach)
{
  // инициализация динамически подгружаемых функций
  QW_INIT(AddListViewItem);
  QW_INIT(InsertListViewItem);
  QW_INIT(RemoveListViewItem);
  QW_INIT(AddListViewItems);
  QW_INIT(SetListViewItemText);
  QW_INIT(GetListViewItemText);
  QW_INIT(SetListViewItemData);
  QW_INIT(GetListViewItemData);
  QW_INIT(GetListViewCount);
  QW_INIT(ClearListView);
  QW_INIT(GetListViewRow);
  QW_INIT(GetListViewCurrentRow);
  QW_INIT(SetListViewCurrentRow);
  QW_INIT(SetListViewSortingEnabled);
  QW_INIT(SortListViewItems);
}

inline QSpxListView::~QSpxListView()
{
  ;
}

// добавление элемента в конец списка
inline int QSpxListView::addItem
(
  const char*   text,                // [in]  текст элемента
  void*         data                 // [in]  данные элемента
)
{
  return QW_CALL(AddListViewItem)(hView, text, data);
}

// вставка элемента после текущего
inline int QSpxListView::insertItem
(
  int           rowAfter,            // [in]  после какого элемента вставить
  const char*   text,                // [in]  текст элемента
  void*         data                 // [in]  данные элемента
)
{
  return QW_CALL(InsertListViewItem)(hView, rowAfter, text, data);
}

// удаление элемента
inline int QSpxListView::removeItem
(
  int           row                  // [in]  какой элемент удалить
)
{
  return QW_CALL(RemoveListViewItem)(hView, row);
}

// добавление набора элементов в конец списка
inline int QSpxListView::addItems
(
  const char**  texts,               // [in]  указатели на тексты элементов
  void**        datas                // [in]  данные элементов
)
{
  return QW_CALL(AddListViewItems)(hView, texts, datas);
}

// установка текста для элемента
inline int QSpxListView::setItemText
(
  int           row,                 // [in]  номер элемента
  const char*   text                 // [in]  текст элемента
)
{
  return QW_CALL(SetListViewItemText)(hView, row, text);
}

// получение текста для элемента
inline const char* QSpxListView::getItemText
(
  int           row                  // [in]  номер элемента
)
{
  return QW_NULLCALL(GetListViewItemText)(hView, row);
}


// установка данного для элемента
inline void QSpxListView::setItemData
(
  int           row,                 // [in]  номер элемента
  void*         data                 // [in]  данные элемента
)
{
  QW_SAFECALL(pSetListViewItemData)(hView, row, data);
}

// получение данного для элемента
inline void* QSpxListView::getItemData
(
  int           row                  // [in]  номер элемента
)
{
  return QW_NULLCALL(GetListViewItemData)(hView, row);
}

// количество элементов
inline int QSpxListView::getCount()
{
  return QW_CALL(GetListViewCount)(hView);
}

// удалить все элементы
inline void QSpxListView::clear()
{
  QW_SAFECALL(pClearListView)(hView);
}

// номер элемента с данным текстом
inline int QSpxListView::getRow
(
  const char*   text,                // [in]  текст элемента
  int           rowStart             // [in]  начальный элемент поиска
)
{
  return QW_CALL(GetListViewRow)(hView, text, rowStart);
}

// номер селектированного элемента
inline int QSpxListView::getCurrentRow()
{
  return QW_CALL(GetListViewCurrentRow)(hView);
}

// установить селекцию на элемент с номером raw
inline void QSpxListView::setCurrentRow
(
  int           row                  // [in]  номер элемента
)
{
  QW_SAFECALL(pSetListViewCurrentRow)(hView, row);
}

// сортировка элементов допустима
inline void QSpxListView::setSortingEnabled
(
  bool          enable               // [in]  разрешить сортировку
)
{
  QW_SAFECALL(pSetListViewSortingEnabled)(hView, enable);
}

// сортировать элементы
inline void QSpxListView::sortItems
(
  bool          orderAcsending       // [in]  сортировка по возростанию / по убыванию 
)
{
  QW_SAFECALL(pSortListViewItems)(hView, orderAcsending);
}


///////////////////////////////////////////////////////////////////////
////  QSpxTreeView class

inline QSpxTreeView::QSpxTreeView(QHandle hView, bool bAttach)  : QSpxView(hView, bAttach)
{
  // инициализация динамически подгружаемых функций
  QW_INIT(GetSnapTreeRoot);
  QW_INIT(AddNode);
  QW_INIT(RemoveNode);
  QW_INIT(GetNodeInfo);
  QW_INIT(SetNodeInfo);
  QW_INIT(FindNode);
  QW_INIT(NodeChecked);
  QW_INIT(CheckNode);
  QW_INIT(GetParentNode);
  QW_INIT(GetFirstChildNode);
  QW_INIT(GetNextChildNode);
  QW_INIT(GetPrevChildNode);
  QW_INIT(GetNodePropertiesCount);
  QW_INIT(GetNodeProperty);
  QW_INIT(SetNodeProperty);
  QW_INIT(GetNodeValue);
  QW_INIT(SetNodeValue);
  QW_INIT(RemoveNodeProperty);
  QW_INIT(RemoveAllNodeProperties);
  QW_INIT(LoadNodeProperties);
  QW_INIT(SaveNodeProperties);
  QW_INIT(LoadNodeProperty);
  QW_INIT(SaveNodeProperty);
  QW_INIT(LoadTreeViewState);
  QW_INIT(SaveTreeViewState);

  QHandle hInstance = QW_CALL(GetInstance)(hView);
  if(hInstance)
  {
    // инициализация корневой вершины
    rootNode.setHandle( QW_CALL(GetSnapTreeRoot)(hInstance) );
    rootNode.setTreeView(this);
  }
}

inline QSpxTreeView::~QSpxTreeView()
{
  for (std::list<QSpxNode*>::iterator it = nodes.begin(); it != nodes.end(); ++it)
  {
    delete *it;
  }
  nodes.clear();
}

// получение корневой вершины
inline QSpxNode* QSpxTreeView::snapTreeRoot()   // [ret] хэндл корневой вершины
{
  return &rootNode;
}

// поиск вершины по текстовому ID
inline QSpxNode* QSpxTreeView::findNode
(
  const char* sNodeId                 // [in]  текстовый id вершины
)
{
  QHandle hNode = QW_CALL(FindNode)(hView, sNodeId);

  if(hNode==QHANDLE_NULL)
    return NULL;

  for(std::list<QSpxNode*>::iterator it=nodes.begin(); it!=nodes.end(); it++)
    if(*it && !strcmp((*it)->id(), sNodeId))
      return *it;

  QSpxNode* pNode = new QSpxNode(hNode, this);
  nodes.push_back(pNode);
  return pNode;
}

///////////////////////////////////////////////////////////////////////
////  QSpxNode class

inline QSpxNode::QSpxNode(QHandle h, QSpxTreeView* p)
{
  hNode = h;
  pView = p;
  if(hNode && pView)
    memcpy(&nodeInfo, info(), sizeof(NodeInfo));
}

  // добавление вершины
inline QSpxNode* QSpxNode::addChildNode(NodeInfo* pNodeInfo)
{
  QHandle hNew = QW_PCALL(pView, AddNode)(hNode, pNodeInfo);
  if(hNew)
  {
    QSpxNode* pNode = new QSpxNode(hNew, pView);
    pView->nodes.push_back(pNode);
    return pNode;
  }
  return NULL;
}

// добавление вершины
inline QSpxNode* QSpxNode::addChildNode(const char* pId, const char* pDescription)
{
  NodeInfo info = {"", "", SN_CHECK, 0};
  strcpy(info.szId, pId);
  strcpy(info.szDescription, pDescription ? pDescription : pId);
  
  // если вершина с таким id уже существует, то возвращаем ее
  QSpxNode* nodeExisted = pView->findNode(pId);
  if(nodeExisted)
    return nodeExisted;

  return addChildNode(&info);
}

// удаление текущей вершины
inline QError QSpxNode::removeNode()
{
  return QERR_NOTIMPLEMENTED;
}

// получение свойств вершины (см. NodeInfo)
inline NodeInfo* QSpxNode::info()
{
  if(QW_PCALL(pView,GetNodeInfo)(hNode, &nodeInfo)==QERR_NO_ERROR)
    return &nodeInfo;
  return NULL;
}

// текстовый идентификатор
inline const char* QSpxNode::id()
{
  if(QW_PCALL(pView,GetNodeInfo)(hNode, &nodeInfo)==QERR_NO_ERROR)
    return nodeInfo.szId;
  return NULL;
}

// текстовое описание
inline const char* QSpxNode::description()
{
  if(QW_PCALL(pView,GetNodeInfo)(hNode, &nodeInfo)==QERR_NO_ERROR)
    return nodeInfo.szDescription;
  return NULL;
}

// тип вершины
inline SnpNodeType QSpxNode::type()
{
  if(QW_PCALL(pView,GetNodeInfo)(hNode, &nodeInfo)==QERR_NO_ERROR)
    return nodeInfo.type;
  return SN_CHECK;
}

// цвет вершины
inline SnpColor QSpxNode::color()
{
  if(QW_PCALL(pView,GetNodeInfo)(hNode, &nodeInfo)==QERR_NO_ERROR)
    return nodeInfo.color;
  return 0;
}

// установка свойств вершины (см. NodeInfo)
inline QError QSpxNode::setInfo(NodeInfo* pInfo)
{
  return QW_PCALL(pView,SetNodeInfo)(hNode, pInfo);
}

// текстовый идентификатор
inline QError QSpxNode::setId(const char* id)
{
  QW_PCALL(pView,GetNodeInfo)(hNode, &nodeInfo);
  strncpy(nodeInfo.szId, id, sizeof(nodeInfo.szId));
  return QW_PCALL(pView,SetNodeInfo)(hNode, &nodeInfo);
}

// текстовое описание
inline QError QSpxNode::setDescription(const char* descr)
{
  QW_PCALL(pView,GetNodeInfo)(hNode, &nodeInfo);
  strncpy(nodeInfo.szDescription, descr, sizeof(nodeInfo.szDescription));
  return QW_PCALL(pView,SetNodeInfo)(hNode, &nodeInfo);
}

// тип вершины
inline QError QSpxNode::setType(SnpNodeType type)
{
  QW_PCALL(pView,GetNodeInfo)(hNode, &nodeInfo);
  nodeInfo.type = type;
  return QW_PCALL(pView,SetNodeInfo)(hNode, &nodeInfo);
}

// цвет вершины
inline QError QSpxNode::setColor(SnpColor color)
{
  QW_PCALL(pView,GetNodeInfo)(hNode, &nodeInfo);
  nodeInfo.color = color;
  return QW_PCALL(pView,SetNodeInfo)(hNode, &nodeInfo);
}

// проверка выделена ли вершина
inline bool QSpxNode::isChecked()
{
  return QW_PCALL(pView,NodeChecked)(hNode) != 0;
}

// выделение вершины
inline QError QSpxNode::setChecked(bool bCheck)
{
  return QW_PCALL(pView,CheckNode)(hNode, bCheck);
}

// получение вершины - родителя
inline QSpxNode* QSpxNode::parent()
{
  QHandle h = QW_PCALL(pView,GetParentNode)(hNode);
  if(h)
  {
    QSpxNode* pNode = new QSpxNode(h, pView);
    pView->nodes.push_back(pNode);  // оптимизировать
    return pNode;
  }
  return NULL;
}

// получение вершины - первого ребенка
inline QSpxNode* QSpxNode::firstChild()
{
  QHandle h = QW_PCALL(pView,GetFirstChildNode)(hNode);
  if(h)
  {
    QSpxNode* pNode = new QSpxNode(h, pView);
    pView->nodes.push_back(pNode);  // оптимизировать
    return pNode;
  }
  return NULL;
}

// получение вершины - следующего брата
inline QSpxNode* QSpxNode::nextBrother()
{
  QHandle hParent = QW_PCALL(pView,GetParentNode)(hNode);
  if(!hParent)
    return NULL;

  QHandle h = QW_PCALL(pView,GetNextChildNode)(hParent, hNode);
  if(h)
  {
    QSpxNode* pNode = new QSpxNode(h, pView);
    pView->nodes.push_back(pNode);  // оптимизировать
    return pNode;
  }
  return NULL;
}

// получение вершины - предыдущего брата
inline QSpxNode* QSpxNode::prevBrother()
{
  QHandle hParent = QW_PCALL(pView,GetParentNode)(hNode);
  if(!hParent)
    return NULL;

  QHandle h = QW_PCALL(pView,GetPrevChildNode)(hParent, hNode);
  if(h)
  {
    QSpxNode* pNode = new QSpxNode(h, pView);
    pView->nodes.push_back(pNode);  // оптимизировать
    return pNode;
  }
  return NULL;
}

////////////////////////////////////////////////////////////
//// Работа со свойствами вершины

// получение количества свойств вершины
inline int QSpxNode::propertiesCount()  // [ret] количество свойств
{
  return int(QW_PCALL(pView,GetNodePropertiesCount)(hNode));
}

// получение свойства вершины
inline QError QSpxNode::property
(
  int               nProperty,       // [in]  номер свойства
  NodePropertyInfo* pNodeProperty    // [out] свойство
)
{
  return QW_PCALL(pView,GetNodeProperty)(hNode, nProperty, pNodeProperty);
}

// установка свойства вершины, значение свойств сохраняются между сеансами
inline QError QSpxNode::setProperty
(
  int               nProperty,       // [in]  номер свойства
  NodePropertyInfo* pNodeProperty    // [in]  свойство
)
{
  return QW_PCALL(pView,SetNodeProperty)(hNode, nProperty, pNodeProperty);
}

// получение свойства вершины
inline const char* QSpxNode::value   // [ret] значение свойства, NULL если свойство не существует
(
  const char* sKey                   // [in]  название свойства
)
{
  if(!pView)
    return 0;

  return pView->pGetNodeValue(hNode, sKey);
}

// установка свойства вершины
inline QError QSpxNode::setValue
(
  const char* sKey,                  // [in]  название свойства
  const char* sValue                 // [in]  значение свойства
)
{
  return QW_PCALL(pView,SetNodeValue)(hNode, sKey, sValue);
}

// удаление свойства
inline QError QSpxNode::removeProperty
(
  const char* sKey                   // [in]  название свойства
)
{
  return QW_PCALL(pView,RemoveNodeProperty)(hNode, sKey);
}

// удаление всех свойств вершины
inline QError QSpxNode::removeAllProperties()
{
  return QW_PCALL(pView,RemoveAllNodeProperties)(hNode);
}


// загрузка свойств вершины
inline QError QSpxNode::loadProperties()
{
  return QW_PCALL(pView,LoadNodeProperties)(hNode);
}

// сохранение свойств вершины
inline QError QSpxNode::saveProperties()
{
  return QW_PCALL(pView,SaveNodeProperties)(hNode);
}

// загрузка свойства вершины
inline QError QSpxNode::loadProperty
(
  const char* sKey                   // [in]  название свойства
)
{
  return QW_PCALL(pView,LoadNodeProperty)(hNode, sKey);
}

// сохранение свойства вершины
inline QError QSpxNode::saveProperty
(
  const char* sKey                   // [in]  название свойства
)
{
  return QW_PCALL(pView,SaveNodeProperty)(hNode, sKey);
}


///////////////////////////////////////////////////////////////////////
////  QSpxToolbar class

inline QSpxToolbar::QSpxToolbar(QHandle hView, bool bAttach)  : QSpxView(hView, bAttach)
{
  // инициализация динамически подгружаемых функций
  QW_INIT(AddControl);
  QW_INIT(RemoveControl);
}

inline QSpxToolbar::~QSpxToolbar()
{
}

inline QHandle QSpxToolbar::addControl
(
  QSnp::CtrlInfo* pControlInfo      // [in]  свойства элемента управления
)
{
  return QW_CALL(AddControl)(hView, pControlInfo);
}

inline QError QSpxToolbar::removeControl
(
  QHandle         hControl          // [in]  хэндл элемента управления
)
{
  return QW_CALL(RemoveControl)(hControl);
}

inline QSpxExternalWidget::QSpxExternalWidget(
  const char* dllname,
  QHandle hHandle
  )
  : m_hLibrary(0), m_dllName(dllname)
{
  m_hView = QHANDLE_NULL;
  
  QHandle hLibrary = hHandle;
  // инициализация динамически подгружаемых функций
  QW_INIT(LoadExternalWidgetDll);
  QW_INIT(CreateCustomWidget);
  QW_INIT(GiveDataToWidget);
  QW_INIT(CloseWidget);

  m_hLibrary = QW_CALL(LoadExternalWidgetDll)(m_dllName.c_str());

  if (!m_hLibrary)
    return;

  m_hView = QW_CALL(CreateCustomWidget)((void*)m_hLibrary);

  return;
}

// деструктор
inline QSpxExternalWidget::~QSpxExternalWidget()
{
  if(m_hView == QHANDLE_NULL)
    return;

  QError err = QW_CALL(CloseWidget)((void*)m_hLibrary, (void*)m_hView);

  if(err != QERR_NO_ERROR)
    return;

  m_hView = QHANDLE_NULL;

  if(m_hLibrary)
  {
    QSpxFreeLibrary(m_hLibrary);
    m_hLibrary = QHANDLE_NULL;
  }
}

inline bool QSpxExternalWidget::readFBData(void* pFrame)
{
  if (!(m_hLibrary && pGiveDataToWidget))
      return QHANDLE_NULL;

  return QW_CALL(GiveDataToWidget)((void*)m_hLibrary, (void*)m_hView, pFrame);
}

#endif // QSPX_IMPL_H
