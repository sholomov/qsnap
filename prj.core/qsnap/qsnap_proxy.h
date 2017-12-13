/**
  \file   qsnp_proxy.h
  \brief  C++ proxy classes for QSnap library
  \author Sholomov D.
  \date   05.06.2013
*/

#pragma once

#ifndef QSNP_PROXY_H
#define QSNP_PROXY_H

#ifdef __GCC__
  #pragma GCC diagnostic ignored "-Weffc++"
#endif // __GCC__

#include <qsnap/qsnap.h>
#include <qsnap/qspx_macro.h>

#include <list>

using namespace QSnp;

//////////////////////////////////////////////////////////////////////////
//// Предварительное объявление классов

class QSpxInstance;
class QSpxView;
class QSpxTreeView;
class QSpxTextView;
class QSpxListView;
class QSpxImageView;
class QSpxSyncImageView;
class QSpxFigure;
class QSpxNode;
class QSpxExternalWidget;

///////////////////////////////////////////////////////////////////////
////  QSnwInstance class

class QSpxInstance
{
public:

  // конструктор аттачем к существующему объекту
  QSpxInstance(
        QHandle hInstance = 0         // [in] хэндл view для присоединения к существующему объекту
        );

  // деструктор
  virtual ~QSpxInstance();

  // инициализация инстанса снэпа
  QError initialize(bool bThreadMode);

  // деинициализация инстанса снэпа
  QError terminate();

  // получение хэндла инстанса снэпа
  QHandle handle();

  // получение хэндла библиотеки снэпа
  QHandle library();

  // присоединение объекта-хэлпера к объекту Instance по хэндлу
  bool attach(QHandle hInstance);

  // отсоединение объекта-хэлпера от объекта Instance
  bool detach();

  // сохранение текущего расположения окошек
  QError saveCurrentViewConfig(
      const char* szConfigName 
  );

  // загрузка конфигурации расположения окошек
  QError loadViewConfig(
      const char* szConfigName
  );

  // создание окон просмотра (редактирования) определенного типа
  QSpxView* createView(
      QSnp::ViewType eViewType,         // [in]  тип окна просмотра
      const char* sId,                  // [in]  идентификатор окна
      long long   parm = 0              // [in]  дополнительный параметр

  );

  // создание окон просмотра (редактирования) определенного типа
  template<class TView> TView* createView(
      const char* sId                   // [in]  идентификатор окна
  );

  // уничтожение окона просмотра
  QError destroyView(
      QSpxView* pView                   // [in]  хэндл окна
  );

  // уничтожение всех окон просмотра
  QError destroyAllViews();

  // перерисовка всех окон просмотра
  bool updateAllViews();

  // ожидание события пользователя без спецификации View
  QError waitUserInput(
      WaitUserFlags waitFlags,          // [in] тип события (mouse, keyboard)
      WaitOptions*  pWaitFlags,         // [in] параметры ожидания (временной интервал и проч.)
      WaitResults*  pWaitResults        // [out] если указатель не нулевой, в WaitResults прописывается детальная информация о событии
  );

  // ожидание действия пользователя
  QHandle wait(                         // [ret] данные события WaitResults.eventData
      long nTimeInterval = 0            // [in] таймаут ожидания в мсек, по умолчанию бесконечность
  );                          

  // регистрация пользовательского события
  QHandle registerEvent(              // [ret] хэндл события
    const char*   sId                 // [in]  идентификатор события
  );

  // получение идентификатора события
  const char* getEventId(             // [ret] идентификатор события
    QHandle       hEvent              // [in]  хэндл события snap
  );

  // получение идентификатора события
  QHandle getEvent(                   // [ret] хэндл события
    const char*   sId                 // [in]  идентификатор события
  );

  // вызов события по идентификатору
  bool fireEvent(
    QHandle       hEvent,             // [in]  хэндл экземпляра snap
    const char*   sParm = 0,          // [in]  основной параметр
    void*         sParmEx = 0         // [in]  дополнительный параметр
  );

  // вызов события по идентификатору
  QHandle fireEvent(
    const char*   sId,                // [in]  идентификатор события
    const char*   sParm = 0,          // [in]  основной параметр
    void*         sParmEx = 0         // [in]  дополнительный параметр
  );

  // ожидание события пользователя со спецификацией View
  QError waitUserInput(
      QHandle hView,                    // [in] хэндл окна, может быть QHANDLE_NULL, тогда события
      WaitUserFlags waitFlags,          // [in] тип события (mouse, keyboard)
      WaitOptions*  pWaitFlags,         // [in] параметры ожидания (временной интервал и проч.)
      WaitResults*  pWaitResults        // [out] если указатель не нулевой, в WaitResults прописывается детальная информация о событии
  );

  // создание пользовательского виджета из внешней dll
  QSpxExternalWidget* createWidget(
    const char* dllname                 // [in]  название внешней dll
  );

protected:
  QHandle hInstance;  /// хэндл инстенса снепа
  QHandle hLibrary;   /// хэндл подгружаемой библиотеки

  bool bAttached;     /// признак того, что не нужно уничтожать объект View    

  // указатели на функции
  QW_DEF_TYPE(Initialize)(bool bThreadMode);
  QW_DEF_FUNC(Initialize);

  QW_DEF_TYPE(Terminate)(QHandle hInstance);
  QW_DEF_FUNC(Terminate);

  QW_DEF_TYPE(SaveCurrentViewConfig)(QHandle hInstance, const char* sConfigName);
  QW_DEF_FUNC(SaveCurrentViewConfig);

  QW_DEF_TYPE(LoadViewConfig)(QHandle hInstance, const char* sConfigName);
  QW_DEF_FUNC(LoadViewConfig);

  QW_DEF_TYPE(CreateView)(QHandle hInstance, QSnp::ViewType eViewType, const char* sId);
  QW_DEF_FUNC(CreateView);

  QW_DEF_TYPE(DestroyView)(QHandle hView);
  QW_DEF_FUNC(DestroyView);

  QW_DEF_TYPE(DestroyAllViews)(QHandle hInstance);
  QW_DEF_FUNC(DestroyAllViews);

  QW_DEF_TYPE(RegisterEvent)(QHandle hInstance, const char* sId);
  QW_DEF_FUNC(RegisterEvent);

  QW_DEF_TYPE_EX(GetEventId,  const char* )(QHandle hEvent);
  QW_DEF_FUNC(GetEventId);

  QW_DEF_TYPE(GetEvent)(QHandle hInstance, const char* sId);
  QW_DEF_FUNC(GetEvent);

  QW_DEF_TYPE(FireEvent)(QHandle hEvent, const char* sParm, void* sParmEx);
  QW_DEF_FUNC(FireEvent);

  QW_DEF_TYPE(FireEventId)(QHandle hInstance, const char* sId, const char* sParm, void* sParmEx);
  QW_DEF_FUNC(FireEventId);

  QW_DEF_TYPE(WaitUserInput)(QHandle hView, WaitUserFlags waitFlags, WaitOptions*  pWaitFlags, WaitResults*  pWaitResults);
  QW_DEF_FUNC(WaitUserInput);


 };

///////////////////////////////////////////////////////////////////////
////  QSpxView class

class QSpxView
{
public:

  // конструктор
  QSpxView(
    QHandle hView = QHANDLE_NULL,       // [in] хэндл view для присоединения к существующему объекту
    bool bAttach = true                 // [in] true при присоединении окна, при этом отсутствует его разрущение в деструкторе
    );

  // деструктор
  virtual ~QSpxView();

  // уничтожение окона просмотра
  QError destroy();

  // получение хэндла окна просмотра
  QHandle handle();

  // присоединение объекта-хэлпера к объекта View по хэндлу
  virtual bool attach(QHandle hView);

  // отсоединение объекта-хэлпера от объекта View
  virtual bool detach();

  // показ/скрытие окна просмотра
  QError show(bool bShow);
  bool  isVisible();

  // перерисовка окна
  QError update(QSnp::SnpRect* pRect = 0);

  // прямоугольник окна
  SnpRect rect();
  void setRect(const SnpRect& rc);

  // текст заголовка
  const char* title();
  void setTitle(const char*);

  // ожидание события пользователя
  QError waitUserInput(
      WaitUserFlags waitFlags,          // [in] тип события (mouse, keyboard)
      WaitOptions*  pWaitFlags,         // [in] параметры ожидания (временной интервал и проч.)
      WaitResults*  pWaitResults        // [out] если указатель не нулевой, в WaitResults прописывается детальная информация о событии
  );

protected:

  friend class QSpxInstance;

  QHandle hView;            /// хэндл окна просмотра
  //QSpxInstance* pInstance;  /// класс инстенса снепа
  QHandle hLibrary;         /// хэндл библиотеки

  bool bAttached;           /// признак того, что не нужно уничтожать объект View    

  // указатели на функции
  QW_DEF_TYPE(DestroyView)(QHandle hView);
  QW_DEF_FUNC(DestroyView);

  QW_DEF_TYPE(ShowView)(QHandle hView, bool bShow);
  QW_DEF_FUNC(ShowView);

  QW_DEF_TYPE(UpdateView)(QHandle hView, SnpRect* pRect);
  QW_DEF_FUNC(UpdateView);

  QW_DEF_TYPE(GetViewInfo)(QHandle hView, ViewInfo* pViewInfo);
  QW_DEF_FUNC(GetViewInfo);

  QW_DEF_TYPE(SetViewInfo)(QHandle hView, ViewInfo* pViewInfo);
  QW_DEF_FUNC(SetViewInfo);

  QW_DEF_TYPE(GetInstance)(QHandle hView);
  QW_DEF_FUNC(GetInstance);

  void setViewHandle(QHandle h) { hView = h; }
  //void setInstanceHandle(QHandle h) { hInstance = h; }

};

///////////////////////////////////////////////////////////////////////
////  QSnwImageView class

class QSpxImageView : public QSpxView
{
public:

  // конструктор
  QSpxImageView(
    QHandle hView = QHANDLE_NULL,       // [in] хэндл view для присоединения к существующему объекту
    bool bAttach = true                 // [in] true при присоединении окна, при этом отсутствует его разрущение в деструкторе
    );

  // деструктор
  virtual ~QSpxImageView();

  // установка изображения
  QError setImage
  (
    const char* sFileName,              // [in]  файл изображения
    QSnp::ImageFlags  flagsShow = 0     // [in]  флаги показа изображения
  );

  // установка изображения cv::Mat
  QError setImage
  (
    const cv::Mat& image,               // [in]  файл изображения
    QSnp::ImageFlags  flagsShow = 0     // [in]  флаги показа изображения
  );

#ifdef __MINIMG__
  // установка изображения MinImg
  QError setImage
  (
    const MinImg* minImage,             // [in]  файл изображения
    QSnp::ImageFlags  flagsShow = 0     // [in]  флаги показа изображения
  );
#endif

/*
  // получение информации об изображении (см. ImageViewInfo)
  QError imageViewInfo
  (
    QHandle         hView,              // [in]  хэндл окна
    ImageViewInfo*  pViewInfo           // [out] свойства окна
  );

  // установка информации об изображении  (см. ImageViewInfo)
  QError setImageViewInfo
  (
    QHandle         hView,              // [in]  хэндл окна
    ImageViewInfo*  pViewInfo           // [in]  свойства окна
  );

  // масштабирование изображения
  QError scaleToRect
  (
    QHandle       hView,              // [in]  хэндл окна
    QSnp::SnpRect*   pRect               // [in]  прямоугольник, масштабируемый в размер окна
  );
*/

  // установка коэффициента масштабирования фигур в координатах изображения
  QError setScaleFactor
  (
    double        scale,              // [in] координата по горизонтали
    const char*   idGroup = 0         // [in] идентификатор группы фигур (not implemented)
  );

  // отрисовка точки
  QSpxFigure* drawPoint         // [ret] хэндл фигуры
  (
    int           x,                  // [in] координата по горизонтали
    int           y,                  // [in] координата по вертикали
    int           ptWidth,            // [in] толщина точки (диаметр)
    SnpColor      color,              // [in] цвет
    const char*   idGroup             // [in] идентификатор группы фигур (для совместной работы)
  );

  // отрисовка линии
  QSpxFigure*  drawLine          // [ret] хэндл фигуры
  (
    int           xFrom,              // [in] координата начала по горизонтали
    int           yFrom,              // [in] координата начала по вертикали
    int           xTo,                // [in] координата конца по горизонтали
    int           yTo,                // [in] координата конца по вертикали
    int           lineWidth,          // [in] толщина линии
    SnpColor      color,              // [in] цвет
    LineType      type,               // [in] тип линии (линия, стрелка, пунктир и т.д.)
    const char*   idGroup = 0,        // [in] идентификатор группы фигур
    bool          bRepaint = false    // [in] отрисовка окна
  );

  // отрисовка прямоугольника
  QSpxFigure* drawRect          // [ret] хэндл фигуры
  (
    SnpRect       rect,               // [in] координаты прямоугольника
    int           lineWidth,          // [in] толщина линии
    SnpColor      color,              // [in] цвет
    LineType      type = LT_LINE,     // [in] тип линии (линия, пунктир и т.д.)
    const char*   idGroup = 0,        // [in] идентификатор группы фигур
    bool          bRepaint = false    // [in] отрисовка окна
    );

  // отрисовка многоугольника, ломанной
  QSpxFigure* drawPolygon       // [ret] хэндл фигуры
  (
    SnpPoint*     points,             // [in] массив точек многоугольника
    int*          szPoints,           // [in,out] размер массива точек
    int           lineWidth,          // [in] толщина линии
    SnpColor      color,              // [in] цвет
    PolygonType   type,               // [in] тип многоугольника (ломаная, замкнутый и т.д.)
    const char*   idGroup = 0,        // [in] идентификатор группы фигур
    bool          bRepaint = false    // [in] отрисовка окна
);

  // отрисовка эллипса
  QSpxFigure* drawEllipse       // [ret] хэндл фигуры
  (
    SnpRect       rect,               // [in] координаты охватывающего прямоугольника
    int           lineWidth,          // [in] толщина линии
    SnpColor      color,              // [in] цвет
    LineType      type,               // [in] тип линии (линия, пунктир и т.д.)
    const char*   idGroup = 0,        // [in] идентификатор группы фигур
    bool          bRepaint = false    // [in] отрисовка окна
  );

  // отрисовка дуги
  QSpxFigure* drawArc           // [ret] хэндл фигуры
  (
    int           lineWidth,          // [in] толщина линии
    SnpColor      color,              // [in] цвет
    LineType      type,               // [in] тип линии (линия, пунктир и т.д.)
    const char*   idGroup = 0,        // [in] идентификатор группы фигур
    bool          bRepaint = false    // [in] отрисовка окна
  );

  // отрисовка текста
  QSpxFigure* drawText          // [ret] хэндл фигуры
  (
    SnpRect       rect,               // [in] координаты охватывающего прямоугольника
    const char*   textValue,          // [in] текстовое значение
    double        fontSize,           // [in] кегль шрифта
    SnpColor      fontColor,          // [in] цвет шрифта
    const char*   fontType = 0,       // [in] тип шрифта
    const char*   idGroup = 0,        // [in] идентификатор группы фигур
    bool          bRepaint = false    // [in] отрисовка окна
  );

  // выставлена ли пользовательская точка
  bool userPointSet();

  // получение координат пользовательской точки
  QSnp::SnpPoint getUserPoint();

  // выставлен ли пользовательский прямоугольник
  bool userRectSet();

  // получение координат пользовательского прямоугольника
  QSnp::SnpRect getUserRect();
  
  // содержит ли прямоугольник пользовательскую точку
  bool containUserPoint(QSnp::SnpRect rc);

  // содержится ли прямоугольник полностью внутри пользовательского прямоугольника
  bool insideUserRect(QSnp::SnpRect rc);

  /*
  // функция показа/скрытия фигуры
  QError showFigure
  (
    QHandle       hFigure,            // [in] хэндл фигуры
    bool          bShow               // [in] показать/скрыть
  );
*/
  // функция удаления всех фигур в данном окне
  QError clearFigures();


protected:

  // указатели на функции
  QW_DEF_TYPE(SetImage)(QHandle hView, const char* sFileName, QSnp::ImageFlags flagsShow);
  QW_DEF_FUNC(SetImage);

  QW_DEF_TYPE(SetMatImage)(QHandle hView, const cv::Mat* matImage, QSnp::ImageFlags flagsShow);
  QW_DEF_FUNC(SetMatImage);

#ifdef __MINIMG__
  QW_DEF_TYPE(SetMinImage)(QHandle hView, const MinImg* minImage, QSnp::ImageFlags flagsShow);
  QW_DEF_FUNC(SetMinImage);
#endif

  QW_DEF_TYPE(GetImageViewInfo)(QHandle hView, QSnp::ImageViewInfo* pViewInfo);
  QW_DEF_FUNC(GetImageViewInfo);

  QW_DEF_TYPE(SetImageViewInfo)(QHandle hView, QSnp::ImageViewInfo* pViewInfo);
  QW_DEF_FUNC(SetImageViewInfo);

  QW_DEF_TYPE(ImageScaleToRect)(QHandle hView, QSnp::SnpRect* pRect);
  QW_DEF_FUNC(ImageScaleToRect);

  QW_DEF_TYPE(SetScaleFactor)
  (
    QHandle       hView,              // [in] хэндл окна
    double        scale,              // [in] координата по горизонтали
    const char*   idGroup             // [in] идентификатор группы фигур (not implemented)
  );
  QW_DEF_FUNC(SetScaleFactor);

  QW_DEF_TYPE(DrawPoint)
  (
    QHandle       hView,              // [in] хэндл окна
    int           x,                  // [in] координата по горизонтали
    int           y,                  // [in] координата по вертикали
    int           ptWidth,            // [in] толщина точки (диаметр)
    SnpColor      color,              // [in] цвет
    const char*   idGroup             // [in] идентификатор группы фигур (для совместной работы)
  );
  QW_DEF_FUNC(DrawPoint);

  // отрисовка линии
  QW_DEF_TYPE(DrawLine)          // [ret] хэндл фигуры
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
  QW_DEF_FUNC(DrawLine);

  // отрисовка прямоугольника
  QW_DEF_TYPE(DrawRect)          // [ret] хэндл фигуры
  (
    QHandle       hView,              // [in] хэндл окна
    QSnp::SnpRect*   pRect,              // [in] координаты прямоугольника
    int           lineWidth,          // [in] толщина линии
    SnpColor      color,              // [in] цвет
    LineType      type,               // [in] тип линии (линия, пунктир и т.д.)
    const char*   idGroup             // [in] идентификатор группы фигур
  );
  QW_DEF_FUNC(DrawRect);

  // отрисовка текста
  QW_DEF_TYPE(DrawText)         // [ret] хэндл фигуры
  (
    QHandle       hView,              // [in] хэндл окна
    SnpRect*      pRect,              // [in] координаты охватывающего прямоугольника
    const char*   textValue,          // [in] текстовое значение
    double        fontSize,           // [in] кегль шрифта
    SnpColor      fontColor,          // [in] цвет шрифта
    const char*   fontType,           // [in] тип шрифта
    const char*   idGroup             // [in] идентификатор группы фигур
  );
  QW_DEF_FUNC(DrawText);

  // отрисовка многоугольника, ломанной
  QW_DEF_TYPE(DrawPolygon)       // [ret] хэндл фигуры
  (
    QHandle       hView,              // [in] хэндл окна
    QSnp::SnpPoint* points,           // [in] массив точек многоугольника
    int*          szPoints,           // [in,out] размер массива точек
    int           lineWidth,          // [in] толщина линии
    SnpColor      color,              // [in] цвет
    PolygonType   type                // [in] тип многоугольника (ломаная, замкнутый и т.д.)
  );
  QW_DEF_FUNC(DrawPolygon);

  // отрисовка эллипса
  QW_DEF_TYPE(DrawEllipse)       // [ret] хэндл фигуры
  (
    QHandle       hView,              // [in] хэндл окна
    QSnp::SnpRect*   pRect,           // [in] координаты охватывающего прямоугольника
    int           lineWidth,          // [in] толщина линии
    SnpColor      color,              // [in] цвет
    LineType      type,               // [in] тип линии (линия, пунктир и т.д.)
    const char*   idGroup             // [in] идентификатор группы фигур
  );
  QW_DEF_FUNC(DrawEllipse);

  // отрисовка дуги
  QW_DEF_TYPE(DrawArc)           // [ret] хэндл фигуры
  (
    QHandle       hView,              // [in] хэндл окна
  //  ...                             // [in] параметры дуги
    int           lineWidth,          // [in] толщина линии
    SnpColor      color,              // [in] цвет
    LineType      type,               // [in] тип линии (линия, пунктир и т.д.)
    const char*   idGroup             // [in] идентификатор группы фигур
  );
  QW_DEF_FUNC(DrawArc);

  // получение координат пользовательской точки
  QW_DEF_TYPE(GetUserPoint)
  (
    QHandle       hView,              // [in]  хэндл окна
    QSnp::SnpPoint*  point            // [out] координаты точки
  );
  QW_DEF_FUNC(GetUserPoint);

  // получение координат пользовательского прямоугольника
  QW_DEF_TYPE(GetUserRect)
  (
    QHandle       hView,              // [in]  хэндл окна
    QSnp::SnpRect*   pRect            // [out] координаты прямоугольника
  );
  QW_DEF_FUNC(GetUserRect);

  // функция показа/скрытия фигуры
  QW_DEF_TYPE(ShowFigure)
  (
    QHandle       hFigure,            // [in] хэндл фигуры
    bool          bShow               // [in] показать/скрыть
  );
  QW_DEF_FUNC(ShowFigure);

  // функция удаления всех фигур в данном окне
  QW_DEF_TYPE(ClearFigures)
  (
    QHandle       hView               // [in] хэндл окна
  );
  QW_DEF_FUNC(ClearFigures);

};

///////////////////////////////////////////////////////////////////////
////  QSpxSyncImageView class

class QSpxSyncImageView : public QSpxImageView
{
public:

  // конструктор
  QSpxSyncImageView(
    QHandle hView = QHANDLE_NULL,       // [in] хэндл view для присоединения к существующему объекту
    bool bAttach = true                 // [in] true при присоединении окна, при этом отсутствует его разрущение в деструкторе
    );

  // деструктор
  virtual ~QSpxSyncImageView();

  // установка изображения в случае нескольких окон показа
  QError setSubImage
  (
    const char* sFileName,              // [in]  файл изображения
    int nSubView,                       // [in]  номер внутреннего окна (для SynqImageView и проч.)
    QSnp::ImageFlags  flagsShow = 0     // [in]  флаги показа изображения
  );

  // установка изображения в случае нескольких окон показа
  QError setSubImage
  (
    const cv::Mat& image,               // [in]  файл изображения
    int nSubView,                       // [in]  номер внутреннего окна (для SynqImageView и проч.)
    QSnp::ImageFlags  flagsShow = 0     // [in]  флаги показа изображения
  );

#ifdef __MINIMG__
  // установка изображения в случае нескольких окон показа
  QError setSubImage
  (
    const MinImg* pMinImage,            // [in]  изображение
    int nSubView,                       // [in]  номер внутреннего окна (для SynqImageView и проч.)
    QSnp::ImageFlags  flagsShow = 0     // [in]  флаги показа изображения
  );
#endif

protected:

  // указатели на функции
  QW_DEF_TYPE(SetSubImage)(QHandle hView, const char* sFileName, int nSubView, QSnp::ImageFlags flagsShow);
  QW_DEF_FUNC(SetSubImage);

  QW_DEF_TYPE(SetSubMatImage)(QHandle hView, const cv::Mat* pImage, int nSubView, QSnp::ImageFlags flagsShow);
  QW_DEF_FUNC(SetSubMatImage);

#ifdef __MINIMG__
  QW_DEF_TYPE(SetSubMinImage)(QHandle hView, const MinImg* pMinImage, int nSubView, QSnp::ImageFlags flagsShow);
  QW_DEF_FUNC(SetSubMinImage);
#endif

};


///////////////////////////////////////////////////////////////////////
////  QSpxTextView class

class QSpxTextView : public QSpxView
{
public:

  // конструктор
  QSpxTextView(
    QHandle hView = QHANDLE_NULL,       // [in] хэндл view для присоединения к существующему объекту
    bool bAttach = true                 // [in] true при присоединении окна, при этом отсутствует его разрущение в деструкторе
    );

  // деструктор
  virtual ~QSpxTextView();

  // вывод текстового сообщения
  QError log
  (
    const char* sText,                 // [in]  текстовый шаблон
    ...                                // [in]  параметры шаблона
  );

  // вывод текстового сообщения отдельной строкой
  QError logLn
  (
    const char* sText,                 // [in]  текстовый шаблон
    ...                                // [in]  параметры шаблона
  );

  // вывод расширенного текстового сообщения
  QError logEx
  (
    const char* sText,                 // [in]  текстовый шаблон
    SnpColor    color,                 // [in]  цвет текста
    FontFlags   flags                  // [in]  параметры шрифта
  );

  // очистка текста
  QError clear();

protected:

  // указатели на функции
  QW_DEF_TYPE(Log)(QHandle hView, const char* sText, ... );
  QW_DEF_FUNC(Log);

  QW_DEF_TYPE(LogLn)(QHandle hView, const char* sText, ... );
  QW_DEF_FUNC(LogLn);

  QW_DEF_TYPE(LogEx)(QHandle hView, const char* sText, SnpColor color, FontFlags flags);
  QW_DEF_FUNC(LogEx);

  QW_DEF_TYPE(ClearTextView)(QHandle hView);
  QW_DEF_FUNC(ClearTextView);
  
};

///////////////////////////////////////////////////////////////////////
////  QSpxListView class

class QSpxListView : public QSpxView
{
public:

  // конструктор
  QSpxListView(
    QHandle hView = QHANDLE_NULL,       // [in] хэндл view для присоединения к существующему объекту
    bool bAttach = true                 // [in] true при присоединении окна, при этом отсутствует его разрущение в деструкторе
    );

  // деструктор
  virtual ~QSpxListView();

  // добавление элемента в конец списка
  int addItem
  (
    const char*   text,                // [in]  текст элемента
    void*         data = 0             // [in]  данные элемента
  );

  // вставка элемента после текущего
  int insertItem
  (
    int           rowAfter,            // [in]  после какого элемента вставить
    const char*   text,                // [in]  текст элемента
    void*         data = 0             // [in]  данные элемента
  );

  // удаление элемента
  int removeItem
  (
    int           row                  // [in]  какой элемент удалить
  );

  // добавление набора элементов в конец списка
  int addItems
  (
    const char**  texts,               // [in]  указатели на тексты элементов
    void**        datas = 0            // [in]  данные элементов
  );

  // установка текста для элемента
  int setItemText
  (
    int           row,                 // [in]  номер элемента
    const char*   text                 // [in]  текст элемента
  );

  // получение текста для элемента
  const char* getItemText
  (
    int           row                  // [in]  номер элемента
  );

  // установка данного для элемента
  void setItemData
  (
    int           row,                 // [in]  номер элемента
    void*         data = 0             // [in]  данные элемента
  );

  // получение данного для элемента
  void* getItemData
  (
    int           row                  // [in]  номер элемента
  );

  // количество элементов
  int getCount();

  // удалить все элементы
  void clear();

  // номер элемента с данным текстом
  int getRow
  (
    const char*   text,                // [in]  текст элемента
    int           rowStart             // [in]  начальный элемент поиска
  );

  // номер селектированного элемента
  int getCurrentRow();

  // установить селекцию на элемент с номером raw
  void setCurrentRow
  (
    int           row                  // [in]  номер элемента
  );

  // сортировка элементов допустима
  void setSortingEnabled
  (
    bool          enable               // [in]  разрешить сортировку
  );

  // сортировать элементы
  void sortItems
  (
    bool          orderAcsending       // [in]  сортировка по возростанию / по убыванию 
  );

protected:

  // указатели на функции
  QW_DEF_TYPE_EX(AddListViewItem, int)(QHandle hView, const char* text, void* data);
  QW_DEF_FUNC(AddListViewItem);

  QW_DEF_TYPE_EX(InsertListViewItem, int)(QHandle hView, int rowAfter, const char* text, void* data);
  QW_DEF_FUNC(InsertListViewItem);

  QW_DEF_TYPE_EX(RemoveListViewItem, int)(QHandle hView, int row);
  QW_DEF_FUNC(RemoveListViewItem);

  QW_DEF_TYPE_EX(AddListViewItems, int)(QHandle hView, const char** texts, void** datas);
  QW_DEF_FUNC(AddListViewItems);

  QW_DEF_TYPE_EX(SetListViewItemText, int)(QHandle hView, int row, const char* text);
  QW_DEF_FUNC(SetListViewItemText);
  
  QW_DEF_TYPE_EX(GetListViewItemText, const char*)(QHandle hView, int row);
  QW_DEF_FUNC(GetListViewItemText);

  QW_DEF_TYPE_EX(SetListViewItemData, void)(QHandle hView, int row, void* data);
  QW_DEF_FUNC(SetListViewItemData);

  QW_DEF_TYPE_EX(GetListViewItemData, void*)(QHandle hView, int row);
  QW_DEF_FUNC(GetListViewItemData);

  QW_DEF_TYPE_EX(GetListViewCount, int)(QHandle hView);
  QW_DEF_FUNC(GetListViewCount);

  QW_DEF_TYPE_EX(ClearListView, void)(QHandle hView);
  QW_DEF_FUNC(ClearListView);

  QW_DEF_TYPE_EX(GetListViewRow, int)(QHandle hView, const char* text, int rowStart);
  QW_DEF_FUNC(GetListViewRow);

  QW_DEF_TYPE_EX(GetListViewCurrentRow, int)(QHandle hView);
  QW_DEF_FUNC(GetListViewCurrentRow);

  QW_DEF_TYPE_EX(SetListViewCurrentRow, void)(QHandle hView, int row);
  QW_DEF_FUNC(SetListViewCurrentRow);

  QW_DEF_TYPE_EX(SetListViewSortingEnabled, void)(QHandle hView, bool enable);
  QW_DEF_FUNC(SetListViewSortingEnabled);

  QW_DEF_TYPE_EX(SortListViewItems, void)(QHandle hView, bool orderAcsending);
  QW_DEF_FUNC(SortListViewItems);

};


///////////////////////////////////////////////////////////////////////
////  QSpxNode class

class QSpxNode
{
public:

  // конструктор/деструктор
  QSpxNode(QHandle h=QHANDLE_NULL, QSpxTreeView* p=0);
  virtual ~QSpxNode() {}

  // добавление вершины
  QSpxNode* addChildNode(NodeInfo* pNodeInfo);

  // добавление вершины
  QSpxNode* addChildNode(const char* pId, const char* pDescription = 0);

  // удаление текущей вершины
  QError removeNode();

  // получение свойств вершины (см. NodeInfo)
  NodeInfo* info();

  // текстовый идентификатор
  const char* id();

  // текстовое описание
  const char* description();

  // тип вершины
  SnpNodeType type();

  // цвет вершины
  SnpColor color();

  // установка свойств вершины (см. NodeInfo)
  QError setInfo(NodeInfo* nodeInfo);

  // текстовый идентификатор
  QError setId(const char* id);

  // текстовое описание
  QError setDescription(const char* descr);

  // тип вершины
  QError setType(SnpNodeType type);

  // цвет вершины
  QError setColor(SnpColor color);

  // проверка выделена ли вершина
  bool isChecked();

  // выделение вершины
  QError setChecked(bool bCheck);

  // получение вершины - родителя
  QSpxNode* parent();

  // получение вершины - первого ребенка
  QSpxNode* firstChild();

  // получение вершины - следующего брата
  QSpxNode* nextBrother();

  // получение вершины - предыдущего брата
  QSpxNode* prevBrother();

  // получение хэндла вершины
  QHandle handle() { return hNode; }

  // установка хэндла вершины
  void setHandle(QHandle h) { hNode=h; }

  // получение окна дерева снепа
  QSpxTreeView* treeView() { return pView; }
  
  // установка окна дерева снепа
  void setTreeView(QSpxTreeView* p) { pView=p; }

////////////////////////////////////////////////////////////
//// Работа со свойствами вершины

  // получение количества свойств вершины
  int propertiesCount();  // [ret] количество свойств
  
  // получение свойства вершины
  QError property
  (
    int               nProperty,       // [in]  номер свойства
    NodePropertyInfo* pNodeProperty    // [out] свойство
  );

  // установка свойства вершины, значение свойств сохраняются между сеансами
  QError setProperty
  (
    int               nProperty,       // [in]  номер свойства
    NodePropertyInfo* pNodeProperty    // [in]  свойство
  );

  // получение свойства вершины
  const char* value   // [ret] значение свойства, NULL если свойство не существует
  (
    const char* sKey                   // [in]  название свойства
  );

  // установка свойства вершины
  QError setValue
  (
    const char* sKey,                  // [in]  название свойства
    const char* sValue                 // [in]  значение свойства
  );

  // удаление свойства
  QError removeProperty
  (
    const char* sKey                   // [in]  название свойства
  );

  // удаление всех свойств вершины
  QError removeAllProperties();

  // загрузка свойств вершины
  QError loadProperties();

  // сохранение свойств вершины
  QError saveProperties();

  // загрузка свойства вершины
  QError loadProperty
  (
    const char* sKey                   // [in]  название свойства
  );

  // сохранение свойства вершины
  QError saveProperty
  (
    const char* sKey                   // [in]  название свойства
  );

protected:
  QSnapNode     hNode;        ///< хэндл окна
  QSpxTreeView* pView;        ///< класс дерева снепа
  NodeInfo      nodeInfo;     ///< свойства вершины

};

///////////////////////////////////////////////////////////////////////
////  QSpxTreeView class

class QSpxTreeView : public QSpxView
{
public:

  // конструктор
  QSpxTreeView(
    QHandle hView = QHANDLE_NULL,       // [in] хэндл view для присоединения к существующему объекту
    bool bAttach = true                 // [in] true при присоединении окна, при этом отсутствует его разрущение в деструкторе
    );

  // деструктор
  virtual ~QSpxTreeView();

  // получение корневой вершины
  QSpxNode* snapTreeRoot();   // [ret] хэндл корневой вершины

  // поиск вершины по текстовому ID
  QSpxNode* findNode
  (
    const char* sNodeId                 // [in]  текстовый id вершины
  );

  // загрузить состояние вершин
  void loadState();

  // сохранить состояние вершин
  void saveState();

protected:
  // указатели на функции
  QW_DEF_TYPE(GetSnapTreeRoot)(QHandle hInstance);
  QW_DEF_FUNC(GetSnapTreeRoot);

  QW_DEF_TYPE(AddNode)(QSnapNode hParentNode, NodeInfo* pNodeInfo);
  QW_DEF_FUNC(AddNode);

  QW_DEF_TYPE(RemoveNode)(QSnapNode hNode);
  QW_DEF_FUNC(RemoveNode);

  QW_DEF_TYPE(GetNodeInfo)(QSnapNode hNode, NodeInfo* nodeInfo);
  QW_DEF_FUNC(GetNodeInfo);

  QW_DEF_TYPE(SetNodeInfo)(QSnapNode hNode, NodeInfo* nodeInfo);
  QW_DEF_FUNC(SetNodeInfo);

  QW_DEF_TYPE(FindNode)(QHandle hSnapView, const char* sNodeId);
  QW_DEF_FUNC(FindNode);

  QW_DEF_TYPE(NodeChecked)(QSnapNode hNode);
  QW_DEF_FUNC(NodeChecked);

  QW_DEF_TYPE(CheckNode)(QSnapNode hNode, bool bCheck);
  QW_DEF_FUNC(CheckNode);

  QW_DEF_TYPE(GetParentNode)(QSnapNode hNode);
  QW_DEF_FUNC(GetParentNode);

  QW_DEF_TYPE(GetFirstChildNode)(QSnapNode hNode);
  QW_DEF_FUNC(GetFirstChildNode);

  QW_DEF_TYPE(GetNextChildNode)(QSnapNode hNodeParent, QSnapNode hNodeCurrChild);
  QW_DEF_FUNC(GetNextChildNode);

  QW_DEF_TYPE(GetPrevChildNode)(QSnapNode hNodeParent, QSnapNode hNodeCurrChild);
  QW_DEF_FUNC(GetPrevChildNode);

  QW_DEF_TYPE(GetNodePropertiesCount)(QSnapNode hNode);
  QW_DEF_FUNC(GetNodePropertiesCount);

  QW_DEF_TYPE(GetNodeProperty)(QSnapNode hNode, int nProperty, NodePropertyInfo* pNodeProperty);
  QW_DEF_FUNC(GetNodeProperty);

  QW_DEF_TYPE(SetNodeProperty)(QSnapNode hNode, int nProperty, NodePropertyInfo* pNodeProperty);
  QW_DEF_FUNC(SetNodeProperty);

  QW_DEF_TYPE_EX(GetNodeValue, const char*)(QSnapNode hNode, const char* sKey);
  QW_DEF_FUNC(GetNodeValue);

  QW_DEF_TYPE(SetNodeValue)(QSnapNode hNode, const char* sKey, const char* sValue);
  QW_DEF_FUNC(SetNodeValue);

  QW_DEF_TYPE(RemoveNodeProperty)(QSnapNode hNode, const char* sKey);
  QW_DEF_FUNC(RemoveNodeProperty);

  QW_DEF_TYPE(RemoveAllNodeProperties)(QSnapNode hNode);
  QW_DEF_FUNC(RemoveAllNodeProperties);

  QW_DEF_TYPE(LoadNodeProperties)(QSnapNode hNode);
  QW_DEF_FUNC(LoadNodeProperties);

  QW_DEF_TYPE(SaveNodeProperties)(QSnapNode hNode);
  QW_DEF_FUNC(SaveNodeProperties);

  QW_DEF_TYPE(LoadNodeProperty)(QSnapNode hNode, const char* sKey);
  QW_DEF_FUNC(LoadNodeProperty);

  QW_DEF_TYPE(SaveNodeProperty)(QSnapNode hNode, const char* sKey);
  QW_DEF_FUNC(SaveNodeProperty);

  QW_DEF_TYPE_EX(LoadTreeViewState, void)(QHandle hView);
  QW_DEF_FUNC(LoadTreeViewState);

  QW_DEF_TYPE_EX(SaveTreeViewState, void)(QHandle hView);
  QW_DEF_FUNC(SaveTreeViewState);

protected:
  QSpxNode rootNode;       ///< корневая вершина
  std::list<QSpxNode*> nodes;  ///< указатели на аллоцированные классы вершин

  friend class QSpxNode;
};

///////////////////////////////////////////////////////////////////////
////  QSpxTextView class

class QSpxToolbar : public QSpxView
{
public:

  // конструктор
  QSpxToolbar(
    QHandle hView = QHANDLE_NULL,       // [in] хэндл view для присоединения к существующему объекту
    bool bAttach = true                 // [in] true при присоединении окна, при этом отсутствует его разрущение в деструкторе
    );

  // деструктор
  virtual ~QSpxToolbar();

  // добавление элемента управления
  QHandle addControl
  (
    QSnp::CtrlInfo* pControlInfo      // [in]  свойства элемента управления
  );

  // удаление элемента управления
  QError removeControl
  (
    QHandle         hControl          // [in]  хэндл элемента управления
  );

protected:
  // указатели на функции
  QW_DEF_TYPE(AddControl)(QHandle hView, QSnp::CtrlInfo* pControlInfo);
  QW_DEF_FUNC(AddControl);

  QW_DEF_TYPE(RemoveControl)(QHandle hControl);
  QW_DEF_FUNC(RemoveControl);

};

///////////////////////////////////////////////////////////////////////
////  QSpxExternalWidget class

class QSpxExternalWidget
{
public:

  // конструктор
  QSpxExternalWidget(
    const char* dllname,
    QHandle handle
    );

  // деструктор
  virtual ~QSpxExternalWidget();

  bool readFBData(void* frame);
protected:
  //friend class QSpxInstance;

  std::string m_dllName;

  QHandle m_hView;            /// хэндл окна просмотра
  
  QHandle m_hLibrary;         /// хэндл библиотеки

  // указатели на функции
  QW_DEF_TYPE(LoadExternalWidgetDll)(const char* dllname);
  QW_DEF_FUNC(LoadExternalWidgetDll);

  QW_DEF_TYPE(CreateCustomWidget)(void* hDll);
  QW_DEF_FUNC(CreateCustomWidget);

  QW_DEF_TYPE(GiveDataToWidget)(void* hDll, void* pW, void* pFrame);
  QW_DEF_FUNC(GiveDataToWidget);

  QW_DEF_TYPE(CloseWidget)(void* hDll, void* hWidget);
  QW_DEF_FUNC(CloseWidget);
};

inline QHandle QSpxLoadLibrary();
inline QHandle QSpxGetFunction(QHandle hLibrary, const char* pFuncName);
inline QError QSpxFreeLibrary(QHandle hLibrary);

#include <qsnap/qspx_impl.h>

#endif // QSNP_PROXY_H
