/**
  \file   qsnap.cpp
  \brief  Functions for QSnap library application programming interface
  \author Sholomov D.
  \date   15.5.2013
*/

#include <qsnap/qsnap.h>
#include <qsnap/qspx_macro.h>
#include <qsnap/qspx_impl.h>

#include <QSharedPointer>
#include <QApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QSettings>
#include <QObject>
#include <QLabel>
#include <QTimer>
#include <QToolBar>
#include <QAction>

#include <atomic>
#include <fstream>

#include "QSnpInstance.h"
#include "QSnpTreeView.h"
#include "QSnpTextView.h"
#include "QSnpImageWidget.h"
#include "QSnpImageView.h"
#include "QSnpSyncImageView.h"
#include "QSnpToolbarView.h"
#include "QSnpToolbar.h"
#include "QSnpListView.h"

#include "eventfilters.h"
#include "thread_safe_queue.h"

using namespace QSnp;
using namespace std;

// Объект QApplication, singleton, создается в Initialize
QSharedPointer<QApplication> qSnapQTApplication;

// Объекты для выполнения функций qsnap в потоке QApplication
static XThreads::XThread_pool* pThreadPool = nullptr;
static std::atomic_bool threadMode;
const int ONE_THREAD = 1;

namespace QSnp {

// Выполнение лямбда-функции cmd в потоке QApplication (при threadMode=true).
// В Qt есть проблема в том, что объекты Qt нельзя вызывать из разных потоков.
// При threadMode=false просто происходит вызов функции cmd
template <typename FunctionType>
void executeCommand(FunctionType cmd)
{
  if (threadMode == true && pThreadPool != nullptr)
  {  
    pThreadPool->submit(cmd);
    int cntTries = 0;
    while (pThreadPool->empty != true)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      cntTries++;
    }
  }
  else
    cmd();
}

QHandle impl_Initialize()
{
  if (QApplication::instance() == nullptr)
  {
    static int argc = 1; static char* argv[1] = {"dummy"};
    qSnapQTApplication = QSharedPointer<QApplication>(new QApplication(argc, argv));

#ifndef _MSC_VER  // Linux or Mac OS X
    qSnapQTApplication->setStyle("Fusion");
#endif

  }
  QSnpInstance* pInstance = new QSnpInstance;
  return (QHandle)pInstance;
}

QSNAP_API QHandle Initialize(bool bThreadMode)
{
  QHandle hInstance = QHANDLE_INVALID;
  threadMode.store(bThreadMode);

  if (threadMode == true)
    pThreadPool = new XThreads::XThread_pool(ONE_THREAD);

  // Определение лямбда-функции и передача ее в XThread_pool
  // для выполнения в потоке QApplication. В Qt есть проблема в том,
  // что объекты Qt нельзя вызывать из разных потоков
  auto cmdInitialize = [&]()
  { 
    hInstance = impl_Initialize(); 
  };
  executeCommand(cmdInitialize);

  return hInstance;
}

QError impl_Terminate(QHandle hInstance)
{
  QSnpInstance* pInstance = (QSnpInstance*)hInstance;
  delete pInstance;

  qSnapQTApplication.reset();

  return QERR_NO_ERROR;
}

QSNAP_API QError Terminate(QHandle hInstance)
{
  QError qerr = QERR_NO_ERROR;

  auto cmdTerminate = [&]()
  { 
    qerr = impl_Terminate(hInstance); 
  };
  executeCommand(cmdTerminate);

  if(pThreadPool)
    delete pThreadPool;
  pThreadPool = nullptr;

  return qerr;
}

QHandle impl_CreateView(QHandle hInstance, ViewType eViewType, const char* sId, long long parm)
{
  QSnpInstance* pInstance = (QSnpInstance*)hInstance;
  string sConfig = string("View_")+=sId;

  switch(eViewType)
  {
  case VT_SNAPTREE_VIEW:
    {
      // Для каждого инстанса должен быть ровно 1 TreeView
      QSnpTreeView* pView = pInstance->getTreeView();
      if(!pView)
      {
        pView = new QSnpTreeView;
        QHandle hView = pView->Create(pInstance);
        pInstance->setTreeView(pView);
        pView->setId(sId);
        pView->setDefaultConfig(sConfig.c_str());
        pView->loadProperties();
        pView->getWidget()->setWindowTitle(sId);
        pView->setInstance(pInstance);
      }
      return pView->getViewHandle();
    }
    break;

  case VT_TEXT_VIEW:
    {
      QSnpTextView* pView = new QSnpTextView(); 
      QHandle hView = pView->Create(pInstance);
      pView->setId(sId);
      pView->setDefaultConfig(sConfig.c_str());
      pView->loadProperties();
      pView->getWidget()->setWindowTitle(sId);
      pView->setInstance(pInstance);
      return pView->getViewHandle();
    }
    break;

  case VT_IMAGE_VIEW:
    {
      QSnpImageView* pView = new QSnpImageView(); 
      QHandle hView = pView->Create(pInstance);
      pView->setId(sId);
      pView->setDefaultConfig(sConfig.c_str());
      pView->loadProperties();
      pView->getWidget()->setWindowTitle(sId);
      pView->getScrollArea()->setWindowTitle(sId);
      pView->setInstance(pInstance);
      return pView->getViewHandle();
    }
    break;

  case VT_SYNC_IMAGE_VIEW:
    {
      QSnpSyncImageView* pView = new QSnpSyncImageView((SyncViewOrientation)parm); 
      QHandle hView = pView->Create(pInstance);
      pView->setId(sId);
      pView->setDefaultConfig(sConfig.c_str());
      pView->loadProperties();
      pView->getFrameWidget()->setWindowTitle(sId);
      pView->setInstance(pInstance);
      return pView->getViewHandle();
    }
    break;

  case VT_TOOLBAR:
    {
      QSnpToolbarView* pView = new QSnpToolbarView(); 
      QHandle hView = pView->Create(pInstance);
      pView->setId(sId);
      pView->setDefaultConfig(sConfig.c_str());
      pView->loadProperties();
      pView->getFrameWidget()->setWindowTitle(sId);
      pView->setInstance(pInstance);
      return pView->getViewHandle();
    }
    break;

  case VT_LIST_VIEW:
    {
      QSnpListView* pView = new QSnpListView(); 
      QHandle hView = pView->Create(pInstance);
      pView->setId(sId);
      pView->setDefaultConfig(sConfig.c_str());
      pView->loadProperties();
      pView->getWidget()->setWindowTitle(sId);
      pView->setInstance(pInstance);
      return pView->getViewHandle();
    }
    break;
  }


  return 1;
}

QSNAP_API QHandle CreateView(QHandle hInstance, ViewType eViewType, const char* sId, long long parm)
{
  QHandle hView = QHANDLE_INVALID;

  auto cmdCreateView = [&]()
  { 
    hView = impl_CreateView(hInstance, eViewType, sId, parm); 
  };
  executeCommand(cmdCreateView);

  return hView;
}

QError impl_DestroyView(QHandle hView)
{
  QSnpView* pView = (QSnpView*)hView;
  string sConfig = string("View_")+=pView->getId().toStdString();
  pView->saveProperties(sConfig.c_str());

  pView->getInstance()->removeView(pView);
  if(pView->getViewType() != VT_UNDEFINED)
    delete pView;

  return QERR_NO_ERROR;
}

QSNAP_API QError DestroyView(QHandle hView)
{
  QError qerr = QERR_NO_ERROR;
  auto cmdDestroyView = [&]()
  { 
    qerr = impl_DestroyView(hView); 
  };
  executeCommand(cmdDestroyView);

  return qerr;
}

QSNAP_API QError ShowView(QHandle hView, bool bShow)
{
  if(hView == QHANDLE_INVALID)
    return QERR_ERROR;

  QSnpView* pView = (QSnpView*)hView;
  pView->showWidget(bShow);

  return 1;
}

// перерисовка прямоугольной зоны окна
QSNAP_API QError UpdateView
(
  QHandle hView,                      // [in]  хэндл окна
  QSnp::SnpRect* pRect                // [in]  зона для перерисовки
)
{
  if(hView == QHANDLE_INVALID)
    return QERR_ERROR;

  QSnpView* pView = (QSnpView*)hView;
  QWidget* pFrame = pView->getFrameWidget();
  QWidget* pWidget = pView->getWidget();

  if(pWidget)
    pWidget->repaint();
  if(pWidget!=pFrame)
    pFrame->repaint();

  return QERR_NO_ERROR;
}

// получение общих свойств окна (координаты и проч.)
QSNAP_API QError GetViewInfo
(
  QHandle   hView,                    // [in]  хэндл окна
  ViewInfo* pViewInfo                 // [out] свойства окна
)
{
  if(hView == QHANDLE_INVALID)
    return QERR_ERROR;

  QSnpView* pView = (QSnpView*)hView;
  QWidget* pFrameWidget = pView->getFrameWidget();
  if(!pFrameWidget)
    return QERR_ERROR;

  pView->setWindowTitleBuff(pFrameWidget->windowTitle().toStdString().c_str());
  pViewInfo->szTitle = pView->getWindowTitleBuff();
  pViewInfo->rc = SnpRect_cast(pFrameWidget->geometry());
  pViewInfo->bVisible = pFrameWidget->isVisible();

  return QERR_NO_ERROR;
}

// установка общих свойств окна (координаты и проч.)
QSNAP_API QError SetViewInfo
(
  QHandle   hView,                    // [in] хэндл окна
  ViewInfo* pViewInfo                 // [in] свойства окна
)
{
  if(hView == QHANDLE_INVALID)
    return QERR_ERROR;

  QSnpView* pView = (QSnpView*)hView;
  //QWidget* pWidget = pView->getWidget();
  QWidget* pFrameWidget = pView->getFrameWidget();
  if(!pFrameWidget)
    return QERR_ERROR;

  pView->setWindowTitleBuff(pViewInfo->szTitle);
  pFrameWidget->setWindowTitle(pViewInfo->szTitle);
  pFrameWidget->setGeometry(QRect_cast(pViewInfo->rc));
  pFrameWidget->repaint();

  if(pViewInfo->bVisible)
    pFrameWidget->show();
  else
    pFrameWidget->hide();

  return QERR_NO_ERROR;
}

QSNAP_API QHandle impl_GetInstance         // [ret] хэндл инстанса
  (
  QHandle   hView                     // [in] хэндл окна
  )
{
  if(hView == QHANDLE_INVALID)
    return QERR_ERROR;

  QSnpView* pView = (QSnpView*)hView;
  return (QHandle)pView->getInstance();
}

QSNAP_API QHandle GetInstance         // [ret] хэндл инстанса
(
  QHandle   hView                     // [in] хэндл окна
)
{
  if(hView == QHANDLE_INVALID)
    return QHANDLE_INVALID;
  
  QHandle hInstance = QHANDLE_INVALID;
  auto cmdGetInstance = [&]()
  { 
    hInstance = impl_GetInstance(hView); 
  };
  executeCommand(cmdGetInstance);

  return hInstance;
}

// загрузка конфигурации
QSNAP_API QError LoadViewConfig(QHandle hInstance, const char* sConfigName)
{
  QSnpInstance* pInstance = (QSnpInstance*)hInstance;
  QWidget* pWidget = pInstance->getTreeView()->getWidget();

  //LoadWindowPos(pWidget, "TreeView");

  return QERR_NO_ERROR;
}

// сохранение текущей конфигурации
QSNAP_API QError SaveCurrentViewConfig(QHandle hInstance, const char* sConfigName)
{
  QSnpInstance* pInstance = (QSnpInstance*)hInstance;
  QWidget* pWidget = pInstance->getTreeView()->getWidget();

  //SaveWindowPos(pWidget, "TreeView");

  return QERR_NO_ERROR;
}

QSNAP_API QError DestroyAllViews(QHandle hInstance)
{
  return 1;
}

QSNAP_API QError ShowAllViews(QHandle hInstance, bool bShow)
{
  return 1;
}

// работа с деревом снэпа
QSNAP_API QSnapNode impl_GetSnapTreeRoot(QHandle hInstance)
{
  QSnpInstance* pInstance = (QSnpInstance*)hInstance;
  QSnpTreeView* pView = pInstance->getTreeView();
  if(!pView)
    return QHANDLE_NULL;
  QSnpRootNode* pRootNode = pView->getRootNode();

  return (QSnapNode)pRootNode;
}

// работа с деревом снэпа
QSNAP_API QSnapNode GetSnapTreeRoot(QHandle hInstance)
{
  QSnapNode hNode = QHANDLE_INVALID;
  auto getSnapTreeRootRun = [&]()
  { 
    hNode = impl_GetSnapTreeRoot(hInstance); 
  };
  executeCommand(getSnapTreeRootRun);
  
  return hNode;
}

QSnapNode impl_AddNode(QSnapNode hParentNode, NodeInfo* nodeInfo)
{
  QSnpNode* pParentNode = (QSnpNode*)hParentNode;
  QSnpNode* pNewNode = pParentNode->addChildNode(*nodeInfo);

  QSnpTreeView* pView = pParentNode->getInstance()->getTreeView();
  if(pView)
    pView->getWidget()->repaint();

  return (QSnapNode)pNewNode;
}

QSNAP_API QSnapNode AddNode(QSnapNode hParentNode, NodeInfo* nodeInfo)
{
  QSnapNode hNode = QHANDLE_INVALID;
  auto cmdAddNode = [&]()
  { 
    hNode = impl_AddNode(hParentNode, nodeInfo); 
  };
  executeCommand(cmdAddNode);

  return hNode;
}

QSNAP_API QError RemoveNode(QSnapNode hNode)
{
  return QERR_NOTIMPLEMENTED;
}

// поиск вершины по текстовому ID
QSNAP_API QSnapNode FindNode
(
  QHandle hSnapView,                  // [in]  хэндл окна
  const char* sNodeId                 // [in]  текстовый id вершины
)
{
  QSnpTreeView* pView = (QSnpTreeView*)hSnapView;
  if(!pView)
    return QHANDLE_NULL;

  QSnpNode* pNode = pView->getRootNode()->findNode(sNodeId);
  return pNode ? pNode->handle() : QHANDLE_NULL;
}

QSNAP_API bool NodeChecked(QSnapNode hNode)
{
  QSnpNode* pNode = (QSnpNode*)hNode;
  if(!pNode)
    return false;
  bool bChecked = pNode->isSelected();
  return bChecked;
}

QSNAP_API QError impl_CheckNode(QSnapNode hNode, bool bChecked)
{
  QSnpNode* pNode = (QSnpNode*)hNode;
  if(!pNode)
    return QERR_ERROR;

  QSnpTreeView* pView = pNode->getInstance() ? pNode->getInstance()->getTreeView() : 0;
  if(pView && pView->getTreeWidget())
    pView->getTreeWidget()->selectItem(pNode->getTreeWidgetItem(), bChecked, false);

  return QERR_NO_ERROR;
}

QSNAP_API QError CheckNode(QSnapNode hNode, bool bChecked)
{
  QError qerr = QERR_NO_ERROR;
  auto cmdCheckNode = [&]()
  { 
    qerr = impl_CheckNode(hNode, bChecked); 
  };
  executeCommand(cmdCheckNode);
  
  return qerr;
}

// получение свойств вершины (см. NodeInfo)
QSNAP_API QError GetNodeInfo
(
  QSnapNode hNode,                    // [in]  хэндл вершины
  QSnp::NodeInfo* nodeInfo            // [out] свойства вершины
)
{
  QSnpNode* pNode = (QSnpNode*)hNode;
  if(!pNode)
    return QERR_ERROR;
  
  memset(nodeInfo,0,sizeof(NodeInfo));
  strncpy(nodeInfo->szId, pNode->id().toStdString().c_str(), QS_STR_BUFF-1);
  strncpy(nodeInfo->szDescription, pNode->caption().toStdString().c_str(), QS_STR_BUFF-1);
  nodeInfo->bChecked = pNode->isSelected();
  
  return QERR_NO_ERROR;
}

// установка свойств вершины (см. NodeInfo)
QSNAP_API QError SetNodeInfo
(
  QSnapNode hNode,                    // [in]  хэндл вершины
  QSnp::NodeInfo* nodeInfo            // [in]  свойства вершины
)
{
  QSnpNode* pNode = (QSnpNode*)hNode;
  if(!pNode)
    return QERR_ERROR;
  
  pNode->setId(nodeInfo->szId);
  pNode->setCaption(nodeInfo->szDescription);
  pNode->setSelected(nodeInfo->bChecked);
  
  return QERR_NO_ERROR;
}

QSNAP_API void impl_LoadTreeViewState
(
  QHandle       hView                // [in]  хэндл окна
)
{
  if(hView==QHANDLE_INVALID)
    return;

  QSnpTreeView* pView = (QSnpTreeView*)hView;
  pView->LoadState();
}

// загрузить состояние вершин
QSNAP_API void LoadTreeViewState
(
  QHandle       hView                // [in]  хэндл окна
)
{  
  auto cmdLoadTreeViewState = [&]()
  { 
    impl_LoadTreeViewState(hView); 
  };
  executeCommand(cmdLoadTreeViewState);
  return;
}

// сохранить состояние вершин
QSNAP_API void SaveTreeViewState
(
  QHandle       hView                // [in]  хэндл окна
)
{
  if(hView==QHANDLE_INVALID)
    return;

  QSnpTreeView* pView = (QSnpTreeView*)hView;
  pView->SaveState();
}

////////////////////////////////////////////////////////
//////    Работа со свойствами вершин
////////////////////////////////////////////////////////

// получение количества свойств вершины
QSNAP_API int GetNodePropertiesCount // [ret] количество свойств
(
  QSnapNode hNode                    // [in]  хэндл вершины
)
{
  QSnpNode* pNode = (QSnpNode*)hNode;
  if(!pNode)
    return QERR_ERROR;

  return pNode->vProperties.size();
}

// получение свойства вершины
QSNAP_API QError GetNodeProperty
(
  QSnapNode         hNode,           // [in]  хэндл вершины
  int               nProperty,       // [in]  номер свойства
  NodePropertyInfo* pNodeProperty    // [out] свойство
)
{
  QSnpNode* pNode = (QSnpNode*)hNode;
  if(!pNode || !pNodeProperty)
    return QERR_ERROR;

  if(nProperty<0 || nProperty>=pNode->vProperties.size())
    return QERR_ERROR;

  *pNodeProperty = pNode->vProperties[nProperty];

  return QERR_NO_ERROR;
}

// установка свойства вершины, значение свойств сохраняются между сеансами
QSNAP_API QError SetNodeProperty
(
  QSnapNode         hNode,           // [in]  хэндл вершины
  int               nProperty,       // [in]  номер свойства
  NodePropertyInfo* pNodeProperty    // [in]  свойство
)
{
  QSnpNode* pNode = (QSnpNode*)hNode;
  if(!pNode)
    return QERR_ERROR;

  if(nProperty<0 || nProperty>=pNode->vProperties.size())
    return QERR_ERROR;

  pNode->vProperties[nProperty] = *pNodeProperty;

  return QERR_NO_ERROR;
}

// получение свойства вершины
QSNAP_API const char* GetNodeValue   // [ret] значение свойства, NULL если свойство не существует
(
  QSnapNode hNode,                   // [in]  хэндл вершины
  const char* sKey                   // [in]  название свойства
)
{
  QSnpNode* pNode = (QSnpNode*)hNode;
  if(!pNode)
    return QERR_ERROR;

  int nPropCount = pNode->vProperties.size();
  for(int nProp = 0; nProp<nPropCount; nProp++)
  {
    NodePropertyInfo& prop = pNode->vProperties[nProp];
    if(!strcmp(prop.sKey, sKey))
      return prop.sValue;
  }
  
  return nullptr;
}

// установка свойства вершины
QSNAP_API QError SetNodeValue
(
  QSnapNode hNode,                   // [in]  хэндл вершины
  const char* sKey,                  // [in]  название свойства
  const char* sValue                 // [in]  значение свойства
)
{
  QSnpNode* pNode = (QSnpNode*)hNode;
  if(!pNode)
    return QERR_ERROR;

  int nPropCount = pNode->vProperties.size();
  for(int nProp = 0; nProp<nPropCount; nProp++)
  {
    NodePropertyInfo& prop = pNode->vProperties[nProp];
    if(!strcmp(prop.sKey, sKey))
    {
      strncpy(prop.sValue,sValue, sizeof(prop.sValue)-1);
      return QERR_NO_ERROR;    
    }
  }

  // если свойство не существует, то добавить его
  NodePropertyInfo propInfo={0};
  strncpy(propInfo.sKey, sKey, sizeof(propInfo.sKey)-1);
  strncpy(propInfo.sValue, sValue, sizeof(propInfo.sValue)-1);
  pNode->vProperties.push_back(propInfo);

  return QERR_ERROR;
}

// удаление свойства
QSNAP_API QError RemoveNodeProperty
(
  QSnapNode hNode,                   // [in]  хэндл вершины
  const char* sKey                   // [in]  название свойства
)
{
  QSnpNode* pNode = (QSnpNode*)hNode;
  if(!pNode)
    return QERR_ERROR;

  QVector<QSnp::NodePropertyInfo> vPropertiesNew;
  int nPropCount = pNode->vProperties.size();
  for(int nProp = 0; nProp<nPropCount; nProp++)
  {
    NodePropertyInfo& prop = pNode->vProperties[nProp];
    if(strcmp(prop.sKey, sKey))
      vPropertiesNew.push_back(prop);
  }

  pNode->vProperties = vPropertiesNew;

  return QERR_NO_ERROR;
}

// удаление всех свойств вершины
QSNAP_API QError RemoveAllNodeProperties
(
  QSnapNode hNode                    // [in]  хэндл вершины
)
{
  QSnpNode* pNode = (QSnpNode*)hNode;
  if(!pNode)
    return QERR_ERROR;

  pNode->vProperties.clear();
  return QERR_NO_ERROR;
}

// загрузка свойств вершины
QSNAP_API QError LoadNodeProperties
(
  QSnapNode hNode                    // [in]  хэндл вершины
)
{
  QSnpNode* pNode = (QSnpNode*)hNode;
  if(!pNode)
    return QERR_ERROR;

  pNode->vProperties.clear();


  QSettings sett("QSnap");
  QString sTreeViewId = pNode->getInstance()->getTreeView()->getId();
  QString sGroup = QString("/")+=QString(sTreeViewId)+=QString("/NodeProps");

  sett.beginGroup(sGroup);
  QStringList	keys = sett.childKeys();
  foreach(QString k, keys)
  {
    QString v = sett.value(k).toString();

    NodePropertyInfo propInfo={0};
    strncpy(propInfo.sKey, k.toStdString().c_str(),sizeof(propInfo.sKey)-1);
    strncpy(propInfo.sValue, v.toStdString().c_str(),sizeof(propInfo.sValue)-1);

    pNode->vProperties.push_back(propInfo);
  }
  
  sett.endGroup();

  return QERR_NO_ERROR;
}

// сохранение свойств вершины
QSNAP_API QError SaveNodeProperties
(
  QSnapNode hNode                    // [in]  хэндл вершины
)
{
  QSnpNode* pNode = (QSnpNode*)hNode;
  if(!pNode)
    return QERR_ERROR;

  QSettings sett("QSnap");
  QString sTreeViewId = pNode->getInstance()->getTreeView()->getId();
  QString sGroup = QString("/")+=QString(sTreeViewId)+=QString("/NodeProps");

  sett.beginGroup(sGroup);

  int nPropCount = pNode->vProperties.size();
  for(int nProp = 0; nProp<nPropCount; nProp++)
  {
    NodePropertyInfo& prop = pNode->vProperties[nProp];
    sett.setValue(QString(prop.sKey), QString(prop.sValue));
  }

  sett.endGroup();

  return QERR_NO_ERROR;
}

// загрузка свойства вершины
QSNAP_API QError LoadNodeProperty
(
  QSnapNode hNode,                   // [in]  хэндл вершины
  const char* sKey                   // [in]  название свойства
)
{
  QSnpNode* pNode = (QSnpNode*)hNode;
  if(!pNode)
    return QERR_ERROR;

  QSettings sett("QSnap");
  QString sTreeViewId = pNode->getInstance()->getTreeView()->getId();
  QString sGroup = QString("/")+=QString(sTreeViewId)+=QString("/NodeProps");

  sett.beginGroup(sGroup);
  
  QString v = sett.value(QString(sKey)).toString();
  SetNodeValue(hNode, sKey, v.toStdString().c_str());

  sett.endGroup();

  return QERR_NO_ERROR;
}

// сохранение свойства вершины
QSNAP_API QError SaveNodeProperty
(
  QSnapNode hNode,                   // [in]  хэндл вершины
  const char* sKey                   // [in]  название свойства
)
{
  QSnpNode* pNode = (QSnpNode*)hNode;
  if(!pNode)
    return QERR_ERROR;

  QSettings sett("QSnap");
  QString sTreeViewId = pNode->getInstance()->getTreeView()->getId();
  QString sGroup = QString("/")+=QString(sTreeViewId)+=QString("/NodeProps");

  sett.beginGroup(sGroup);

  int nPropCount = pNode->vProperties.size();
  for(int nProp = 0; nProp<nPropCount; nProp++)
  {
    NodePropertyInfo& prop = pNode->vProperties[nProp];
    if(!strcmp(pNode->vProperties[nProp].sKey, sKey))
      sett.setValue(QString(prop.sKey), QString(prop.sValue));
  }

  sett.endGroup();

  return QERR_NO_ERROR;
}

// вывод текстового сообщения в виде отдельной строки
QSNAP_API QError impl_LogView(QHandle hView, const char* strbuf)
{
  QSnpTextView* pText = (QSnpTextView*)hView;
  pText->Log( strbuf );

  return QERR_NO_ERROR;
}

QSNAP_API QError Log(QHandle hView, const char* sText, ...)
{
  char strbuf[4096]; strbuf[0]=0;
  va_list list;
  va_start(list, sText );
  vsprintf( strbuf, sText, list );
  va_end( list );

  QError qerr = QERR_NO_ERROR;
  auto cmdLogView = [&]()
  { 
    qerr = impl_LogView(hView, strbuf); 
  };
  executeCommand(cmdLogView);
  return QERR_NO_ERROR;
}

// вывод текстового сообщения в виде отдельной строки
QSNAP_API QError LogLn(QHandle hView, const char* sText, ...)
{
  char strbuf[4096]; strbuf[0]=0;
  va_list list;
  va_start(list, sText );
  vsprintf( strbuf, sText, list );
  strcat(strbuf,"\n");
  va_end( list );

  QError qerr = QERR_NO_ERROR;
  auto cmdLogViewLn = [&]()
  {
    qerr = impl_LogView(hView, strbuf); 
  };
  executeCommand(cmdLogViewLn);
  return qerr;
}

QSNAP_API QError LogEx
(
  QHandle     hView,                 // [in]  хэндл окна
  const char* sText,                 // [in]  текстовый шаблон
  SnpColor    color,                 // [in]  цвет текста
  FontFlags   flags                  // [in]  параметры шрифта
)
{
  return Log(hView, sText);
}

QSNAP_API QError impl_ClearTextView
  (
  QHandle     hView                  // [in]  хэндл окна
  )
{
  QSnpTextView* pText = (QSnpTextView*)hView;
  pText->Clear();

  return QERR_NO_ERROR;
}

// очистка текста в текстовом окне
QSNAP_API QError ClearTextView
(
  QHandle     hView                  // [in]  хэндл окна
)
{
  QError qerr = QERR_NO_ERROR;
  auto cmdClearTextView = [&]()
  {
    qerr = impl_ClearTextView(hView); 
  };
  executeCommand(cmdClearTextView);
  return qerr;
}

// установка изображения
QSNAP_API QError impl_SetImage
  (
  QHandle     hView,                 // [in]  хэндл окна
  const char* sFileName,             // [in]  файл изображения
  ImageFlags  flagsShow              // [in]  флаги показа изображения
  )
{
  if(hView==QHANDLE_INVALID)
    return QERR_ERROR;
  QSnpImageView* pView = (QSnpImageView*)hView;

  bool bRepaint = !(flagsShow & IF_DONT_REPAINT);
  pView->setImage(sFileName, bRepaint);

  return QERR_NO_ERROR;
}

// установка изображения
QSNAP_API QError SetImage
(
  QHandle     hView,                 // [in]  хэндл окна
  const char* sFileName,             // [in]  файл изображения
  ImageFlags  flagsShow              // [in]  флаги показа изображения
)
{
  QError qerr = QERR_NO_ERROR;
  auto cmdSetImage = [&]()
  {
    qerr = impl_SetImage(hView, sFileName, flagsShow); 
  };
  executeCommand(cmdSetImage);
  return qerr;
}

// установка изображения
QSNAP_API QError impl_SetSubImage
  (
  QHandle     hView,                 // [in]  хэндл окна
  const char* sFileName,             // [in]  файл изображения
  int         nSubView,              // [in]  номер внутреннего окна (для SynqImageView и проч.)
  ImageFlags  flagsShow              // [in]  флаги показа изображения
  )
{
  if(hView==QHANDLE_INVALID)
    return QERR_ERROR;
  QSnpView* pView = (QSnpView*)hView;
  if(pView->getViewType()!=VT_SYNC_IMAGE_VIEW)
    return QERR_ERROR;

  QSnpSyncImageView* pSyncView = (QSnpSyncImageView*)pView;
  pSyncView->setSubImage(sFileName, nSubView);
  return QERR_NO_ERROR;
}

// установка изображения
QSNAP_API QError SetSubImage
(
  QHandle     hView,                 // [in]  хэндл окна
  const char* sFileName,             // [in]  файл изображения
  int         nSubView,              // [in]  номер внутреннего окна (для SynqImageView и проч.)
  ImageFlags  flagsShow              // [in]  флаги показа изображения
)
{
  QError qerr = QERR_NO_ERROR;
  auto cmdSetSubImage = [&]()
  {
    qerr = impl_SetSubImage(hView, sFileName, nSubView, flagsShow); 
  };
  executeCommand(cmdSetSubImage);
  return qerr;
}

// установка изображения
QSNAP_API QError impl_SetMatImage
  (
  QHandle         hView,              // [in]  хэндл окна
  const cv::Mat*  pImage,             // [in]  изображение cv::Mat
  ImageFlags      flagsShow           // [in]  флаги показа изображения
  )
{
  if(hView==QHANDLE_INVALID || !pImage)
    return QERR_ERROR;
  QSnpImageView* pView = (QSnpImageView*)hView;

  bool bRepaint = !(flagsShow & IF_DONT_REPAINT);
  pView->setImage(*pImage, bRepaint);

  return QERR_NO_ERROR;
}

// установка изображения
QSNAP_API QError SetMatImage
(
  QHandle         hView,              // [in]  хэндл окна
  const cv::Mat*  pImage,             // [in]  изображение cv::Mat
  ImageFlags      flagsShow           // [in]  флаги показа изображения
)
{
  QError qerr = QERR_NO_ERROR;
  auto cmdSetMatImage = [&]()
  {
    qerr = impl_SetMatImage(hView, pImage, flagsShow); 
  };
  executeCommand(cmdSetMatImage);
  return qerr;
}

QSNAP_API QError impl_SetSubMatImage
  (
  QHandle       hView,               // [in]  хэндл окна
  const cv::Mat*  pImage,            // [in]  изображение cv::Mat
  int           nSubView,            // [in]  номер внутреннего окна (для SynqImageView и проч.)
  ImageFlags    flagsShow            // [in]  флаги показа изображения
  )
{
  if(hView==QHANDLE_INVALID)
    return QERR_ERROR;
  QSnpView* pView = (QSnpView*)hView;
  if(pView->getViewType()!=VT_SYNC_IMAGE_VIEW)
    return QERR_ERROR;

  QSnpSyncImageView* pSyncView = (QSnpSyncImageView*)pView;
  pSyncView->setSubImage(*pImage, nSubView);

  return QERR_NO_ERROR;
}

QSNAP_API QError SetSubMatImage
(
  QHandle       hView,               // [in]  хэндл окна
  const cv::Mat*  pImage,            // [in]  изображение cv::Mat
  int           nSubView,            // [in]  номер внутреннего окна (для SynqImageView и проч.)
  ImageFlags    flagsShow            // [in]  флаги показа изображения
)
{
  QError qerr = QERR_NO_ERROR;
  auto cmdSetSubMatImage = [&]()
  {
    qerr = impl_SetSubMatImage(hView, pImage, nSubView, flagsShow); 
  };
  executeCommand(cmdSetSubMatImage);
  return qerr;
}

#ifdef __MINIMG__

// установка изображения хранимого в MinImg
QSNAP_API QError SetMinImage
(
  QHandle       hView,              // [in]  хэндл окна
  const MinImg* minImage,           // [in]  изображение
  ImageFlags    flagsShow           // [in]  флаги показа изображения
)
{
  if(hView==QHANDLE_INVALID)
    return QERR_ERROR;
  QSnpImageView* pView = (QSnpImageView*)hView;
  pView->setImage(minImage);
  return QERR_NO_ERROR;
}

QSNAP_API QError SetSubMinImage
(
  QHandle       hView,               // [in]  хэндл окна
  const MinImg* pMinImage,           // [in]  изображение
  int           nSubView,            // [in]  номер внутреннего окна (для SynqImageView и проч.)
  ImageFlags    flagsShow            // [in]  флаги показа изображения
)
{
  if(hView==QHANDLE_INVALID)
    return QERR_ERROR;
  QSnpView* pView = (QSnpView*)hView;
  if(pView->getViewType()!=VT_SYNC_IMAGE_VIEW)
    return QERR_ERROR;

  QSnpSyncImageView* pSyncView = (QSnpSyncImageView*)pView;
  pSyncView->setSubImage(pMinImage, nSubView);
  return QERR_NO_ERROR;
}

#endif


////////////////////////////////////////////////////////////////////////////////
////   Работа с окном списка QListView

// добавление элемента в конец списка
QSNAP_API int AddListViewItem
(
  QHandle       hView,               // [in]  хэндл окна
  const char*   text,                // [in]  текст элемента
  void*         data                 // [in]  данные элемента
)
{
  if(hView==QHANDLE_INVALID)
    return QHANDLE_INVALID;

  QSnpListView* pView = (QSnpListView*)hView;
  return pView->AddItem(text, data);
}

// вставка элемента после текущего
QSNAP_API int InsertListViewItem
(
  QHandle       hView,               // [in]  хэндл окна
  int           rowAfter,            // [in]  после какого элемента вставить
  const char*   text,                // [in]  текст элемента
  void*         data                 // [in]  данные элемента
)
{
  if(hView==QHANDLE_INVALID)
    return QHANDLE_INVALID;

  QSnpListView* pView = (QSnpListView*)hView;
  return pView->InsertItem(rowAfter, text, data);
}

// удаление элемента
QSNAP_API int RemoveListViewItem
(
  QHandle       hView,               // [in]  хэндл окна
  int           row                  // [in]  какой элемент удалить
)
{
  if(hView==QHANDLE_INVALID)
    return QHANDLE_INVALID;

  QSnpListView* pView = (QSnpListView*)hView;
  pView->RemoveItem(row);
  return QERR_NO_ERROR;
}

// добавление набора элементов в конец списка
QSNAP_API int AddListViewItems
(
  QHandle       hView,               // [in]  хэндл окна
  const char**  texts,               // [in]  указатели на тексты элементов
  void**        datas                // [in]  данные элементов
)
{
  if(hView==QHANDLE_INVALID)
    return QHANDLE_INVALID;

  QSnpListView* pView = (QSnpListView*)hView;
  pView->AddItems(texts);
  return QERR_NO_ERROR;
}

// установка текста для элемента
QSNAP_API int SetListViewItemText
(
  QHandle       hView,               // [in]  хэндл окна
  int           row,                 // [in]  номер элемента
  const char*   text                 // [in]  текст элемента
)
{
  if(hView==QHANDLE_INVALID)
    return QHANDLE_INVALID;

  QSnpListView* pView = (QSnpListView*)hView;
  pView->SetItemText(row, text);
  return QERR_NO_ERROR;
}

// получение текста для элемента
QSNAP_API const char* GetListViewItemText
(
  QHandle       hView,               // [in]  хэндл окна
  int           row                  // [in]  номер элемента
)
{
  if(hView==QHANDLE_INVALID)
    return QHANDLE_INVALID;

  QSnpListView* pView = (QSnpListView*)hView;
  return pView->GetItemText(row);
}

// установка данного для элемента
QSNAP_API void SetListViewItemData
(
  QHandle       hView,               // [in]  хэндл окна
  int           row,                 // [in]  номер элемента
  void*         data                 // [in]  данные элемента
)
{
  if(hView==QHANDLE_INVALID)
    return;

  QSnpListView* pView = (QSnpListView*)hView;
  return pView->SetItemData(row, data);
}

// получение данного для элемента
QSNAP_API void* GetListViewItemData
(
  QHandle       hView,               // [in]  хэндл окна
  int           row                  // [in]  номер элемента
)
{
  if(hView==QHANDLE_INVALID)
    return QHANDLE_INVALID;

  QSnpListView* pView = (QSnpListView*)hView;
  return pView->GetItemData(row);
}

// количество элементов
QSNAP_API int GetListViewCount
(
  QHandle       hView                // [in]  хэндл окна
)
{
  if(hView==QHANDLE_INVALID)
    return QHANDLE_INVALID;

  QSnpListView* pView = (QSnpListView*)hView;
  return pView->Count();
}

// удалить все элементы
QSNAP_API void ClearListView
(
  QHandle       hView                // [in]  хэндл окна
)
{
  if(hView==QHANDLE_INVALID)
    return;

  QSnpListView* pView = (QSnpListView*)hView;
  pView->Clear();
}

// номер элемента с данным текстом
QSNAP_API int GetListViewRow
(
  QHandle       hView,               // [in]  хэндл окна
  const char*   text,                // [in]  текст элемента
  int           rowStart             // [in]  начальный элемент поиска
)
{
  if(hView==QHANDLE_INVALID)
    return QHANDLE_INVALID;

  QSnpListView* pView = (QSnpListView*)hView;
  return pView->Row(text, rowStart);
}

// номер селектированного элемента
QSNAP_API int GetListViewCurrentRow
(
  QHandle       hView                // [in]  хэндл окна
)
{
  if(hView==QHANDLE_INVALID)
    return -1;

  QSnpListView* pView = (QSnpListView*)hView;
  return pView->GetCurrentRow();
}

// установить селекцию на элемент с номером raw
QSNAP_API void SetListViewCurrentRow
(
  QHandle       hView,               // [in]  хэндл окна
  int           row                  // [in]  номер элемента
)
{
  if(hView==QHANDLE_INVALID)
    return;

  QSnpListView* pView = (QSnpListView*)hView;
  pView->SetCurrentRow(row);
}

// установить селекцию на элемент с номером raw
QSNAP_API void SetListViewSortingEnabled
(
  QHandle       hView,               // [in]  хэндл окна
  bool          enable               // [in]  разрешить сортировку
)
{
  if(hView==QHANDLE_INVALID)
    return;

  QSnpListView* pView = (QSnpListView*)hView;
  pView->SetSortingEnabled(enable);
}

// номер элемента с данным текстом
QSNAP_API void SortListViewItems
(
  QHandle       hView,               // [in]  хэндл окна
  bool          orderAcsending       // [in]  сортировка по возростанию / по убыванию 
)
{
  if(hView==QHANDLE_INVALID)
    return;

  QSnpListView* pView = (QSnpListView*)hView;
  pView->SortItems(orderAcsending);
}

////////////////////////////////////////////////////////////////////////////////
////  Отрисовка фигур 

// установка коэффициента масштабирования фигур в координатах изображения
QSNAP_API QHandle SetScaleFactor
(
  QHandle       hView,              // [in] хэндл окна
  double        scale,              // [in] коэффициент масштабирования
  const char*   idGroup             // [in] идентификатор группы фигур (not implemented)
)
{
  if(hView==QHANDLE_INVALID)
    return QHANDLE_INVALID;

  QSnpImageView* pView = (QSnpImageView*)hView;
  return pView->setScaleFactor(scale, idGroup);
}

// отрисовка точки
QSNAP_API QHandle DrawPoint         // [ret] хэндл фигуры
(
  QHandle       hView,              // [in] хэндл окна
  int           x,                  // [in] координата по горизонтали
  int           y,                  // [in] координата по вертикали
  int           ptWidth,            // [in] толщина точки (диаметр)
  SnpColor      color,              // [in] цвет
  const char*   idGroup             // [in] идентификатор группы фигур (для совместной работы)
)
{
  if(hView==QHANDLE_INVALID)
    return QHANDLE_INVALID;

  QSnpImageView* pView = (QSnpImageView*)hView;
  return pView->addPoint(x, y, ptWidth, color, idGroup);
}

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
)
{
  if(hView==QHANDLE_INVALID)
    return QHANDLE_INVALID;

  QSnpImageView* pView = (QSnpImageView*)hView;
  return pView->addLine(xFrom, yFrom, xTo, yTo, lineWidth, color, type, idGroup);
}

// отрисовка прямоугольника
QSNAP_API QHandle DrawRect          // [ret] хэндл фигуры
(
  QHandle         hView,            // [in] хэндл окна
  QSnp::SnpRect*  pRect,            // [in] координаты прямоугольника
  int             lineWidth,        // [in] толщина линии
  SnpColor        color,            // [in] цвет
  LineType        type,             // [in] тип линии (линия, пунктир и т.д.)
  const char*     idGroup           // [in] идентификатор группы фигур
)
{
  if(hView==QHANDLE_INVALID)
    return QHANDLE_INVALID;

  QSnpImageView* pView = (QSnpImageView*)hView;
  return pView->addRect(pRect, lineWidth, color, type, idGroup);
}

// отрисовка прямоугольника
QSNAP_API QHandle DrawEllipse       // [ret] хэндл фигуры
(
  QHandle       hView,              // [in] хэндл окна
  QSnp::SnpRect* pRect,             // [in] координаты прямоугольника
  int           lineWidth,          // [in] толщина линии
  SnpColor      color,              // [in] цвет
  LineType      type,               // [in] тип линии (линия, пунктир и т.д.)
  const char*   idGroup             // [in] идентификатор группы фигур
)
{
  if(hView==QHANDLE_INVALID)
    return QHANDLE_INVALID;

  QSnpImageView* pView = (QSnpImageView*)hView;
  return pView->addEllipse(pRect, lineWidth, color, type, idGroup);
}

// отрисовка прямоугольника
QSNAP_API QHandle DrawArc           // [ret] хэндл фигуры
(
  QHandle       hView,              // [in] хэндл окна
//  ...                             // [in] параметры дуги
  int           lineWidth,          // [in] толщина линии
  SnpColor      color,              // [in] цвет
  LineType      type,               // [in] тип линии (линия, пунктир и т.д.)
  const char*   idGroup             // [in] идентификатор группы фигур
)
{
  if(hView==QHANDLE_INVALID)
    return QHANDLE_INVALID;

  return QERR_NOTIMPLEMENTED;
}

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
)
{
  if(hView==QHANDLE_INVALID)
    return QHANDLE_INVALID;

  QSnpImageView* pView = (QSnpImageView*)hView;
  return pView->addText(pRect, textValue, fontSize, fontColor, fontType, idGroup);
}

// функция показа/скрытия фигуры
QSNAP_API QError ShowFigure
(
  QHandle       hFigure,            // [in] хэндл фигуры
  bool          bShow               // [in] показать/скрыть
)
{
  if(hFigure==QHANDLE_INVALID)
    return QERR_ERROR;

  QSnpFigure* pFigure = (QSnpFigure*)hFigure;
  pFigure->setVisible(bShow);

  return QERR_NO_ERROR;
}

// функция удаления всех фигур в данном окне
QSNAP_API QError ClearFigures
(
  QHandle       hView               // [in] хэндл окна
)
{
  if(hView==QHANDLE_INVALID)
    return QHANDLE_INVALID;

  QSnpImageView* pView = (QSnpImageView*)hView;
  return pView->clearFigures();
}

// получение координат пользовательской точки
QSNAP_API QError GetUserPoint
(
  QHandle       hView,              // [in]  хэндл окна
  QSnp::SnpPoint*  point            // [out] координаты точки
)
{
  if(hView==QHANDLE_INVALID)
    return QHANDLE_INVALID;

  QSnpImageView* pView = (QSnpImageView*)hView;
  return pView->getUserPoint(point);
}

// получение координат пользовательского прямоугольника
QSNAP_API QError GetUserRect
(
  QHandle         hView,            // [in]  хэндл окна
  QSnp::SnpRect*  rect              // [out] координаты прямоугольника
)
{
  if(hView==QHANDLE_INVALID)
    return QHANDLE_INVALID;

  QSnpImageView* pView = (QSnpImageView*)hView;
  return pView->getUserRect(rect);
}

// корректнный выход из функции WaitUserInput
// не двигать в pool!
QError Exit_WaitUserInput(QError err, UserInputEventFilter* evf, QTimer* timer)
{
  qApp->removeEventFilter(evf);
  if(timer)
  {
    timer->stop();
    delete timer;
  }
  return err;
}

////////////////////////////////////////////////////////
//////    пользовательские события
////////////////////////////////////////////////////////

QSNAP_API QHandle impl_RegisterEvent     // [ret] хэндл события
  (
  QHandle       hInstance,          // [in]  хэндл экземпляра snap
  const char*   sId                 // [in]  идентификатор события
  )
{
  if(hInstance==QHANDLE_INVALID)
    return QHANDLE_INVALID;

  QSnpInstance* pInstance = (QSnpInstance*)hInstance;
  return pInstance->addEvent(sId);
}

// регистрация пользовательского события
QSNAP_API QHandle RegisterEvent     // [ret] хэндл события
(
  QHandle       hInstance,          // [in]  хэндл экземпляра snap
  const char*   sId                 // [in]  идентификатор события
)
{
  if(hInstance==QHANDLE_INVALID)
    return QHANDLE_INVALID;

  QHandle hEvent = QHANDLE_INVALID;
  auto cmdRegisterEvent = [&]()
  {
    hEvent = impl_RegisterEvent(hInstance, sId);
  };
  executeCommand(cmdRegisterEvent);
  return hEvent;
}

// получение идентификатора события
QSNAP_API const char* GetEventId    // [ret] идентификатор события
(
  QHandle       hEvent              // [in]  хэндл события snap
)
{
  QSnpEvent* pEvent=(QSnpEvent*)hEvent;
  if(!hEvent)
    return QHANDLE_INVALID;

  QSnpInstance* pInstance = pEvent->getInstance();
  if(!pInstance)
    return QHANDLE_INVALID;

  QList<QSnpEvent>& lstEvents = pInstance->lstFiredEvents;
  for(QList<QSnpEvent>::iterator it=lstEvents.begin(); it!=lstEvents.end(); it++)
  {
    if(it->handle()==hEvent)
      return it->sId.toStdString().c_str();
  }
  return NULL;
}

// получение идентификатора события
QSNAP_API QHandle GetEvent          // [ret] хэндл события
(
  QHandle       hInstance,          // [in]  хэндл экземпляра snap
  const char*   sId                 // [in]  идентификатор события
)
{
  return QHANDLE_NULL;
}

// вызов события по идентификатору
QSNAP_API QHandle FireEvent
(
  QHandle       hEvent,             // [in]  хэндл экземпляра snap
  const char*   sParm,              // [in]  основной параметр
  void*         sParmEx             // [in]  дополнительный параметр
)
{
  QSnpEvent* pEvent=(QSnpEvent*)hEvent;
  if(!hEvent)
    return QHANDLE_INVALID;

  QSnpInstance* pInstance = pEvent->getInstance();
  if(!pInstance)
    return QHANDLE_INVALID;

  pInstance->lstFiredEvents.push_back(*pEvent);
  
  // уведомление о событии
  QTimerEvent evn(0);
  qApp->sendEvent(qApp, &evn);

  return QHANDLE_NULL;
}

// вызов события по идентификатору
QSNAP_API QHandle FireEventId
(
  QHandle       hInstance,          // [in]  хэндл экземпляра snap
  const char*   sId,                // [in]  идентификатор события
  const char*   sParm,              // [in]  основной параметр
  void*         sParmEx             // [in]  дополнительный параметр
)
{
  if(hInstance==QHANDLE_INVALID)
    return QHANDLE_INVALID;

  QSnpInstance* pInstance = (QSnpInstance*)hInstance;
  QSnpEvent* pEvent=(QSnpEvent*)pInstance->getEvent(sId);
  if(!pEvent)
    return QHANDLE_INVALID;

  FireEvent(pEvent->handle(), sParm, sParmEx );

  return QHANDLE_NULL;
}

// ожидание действия со стороны пользователя
QError impl_WaitUserInput
(
  QHandle       hView,              // [in] хэндл окна, может быть NULL, тогда события  
  WaitUserFlags waitFlags,          // [in] тип события (mouse, keyboard)
  WaitOptions*  pWaitOptions,       // [in] параметры ожидания (временной интервал и проч.)
  WaitResults*  pWaitResults        // [out] если указатель не нулевой, в WaitResults прописывается детальная информация о событии
)
{
  if(hView==QHANDLE_NULL)
    return QERR_ERROR;

  QSnpInstance* pInstance = (QSnpInstance*)(hView);
  
  QEventLoop eventLoop;
  eventLoop.processEvents(QEventLoop::AllEvents); // первичное обнуление очереди скопившихся сообщений во избежание задваивания

  // обновление всех view-окон
  pInstance->updateViews();

  // умолчательный интервал для обработки пользовательских событий
  if((waitFlags & WF_USER) && !(waitFlags & WF_TIMER) && pWaitOptions->nTimeInterval==0)
    pWaitOptions->nTimeInterval = UP_DEFAULT_TIME;

  UserInputEventFilter evf(waitFlags, pWaitOptions, qApp);
  evf.setInstance(pInstance);
  QTimer* timer = NULL;

  if(pWaitOptions && pWaitOptions->nTimeInterval!=0) // указан интервал
  {
    timer = new QTimer(qApp);
    timer->connect(timer, SIGNAL(timeout()), &evf, SLOT(onTime()));
    timer->start(pWaitOptions->nTimeInterval);
    evf.setTimeInterval(pWaitOptions->nTimeInterval);
  }
  
  qApp->installEventFilter(&evf);
  
  // обработка цикла сообщений
  while(true)
  {
    eventLoop.processEvents(QEventLoop::AllEvents);
    if(evf.eventSignaled() == true)
    {
      QEvent* pEvent = evf.getEvent();
      QEvent::Type type = pEvent->type();

      if(pWaitResults)
      {
        switch(type)
        {
        case QEvent::Destroy:
        case QEvent::Close:
        case QEvent::Quit:
          return Exit_WaitUserInput(QERR_NO_ERROR, &evf, timer);

        case QEvent::MouseButtonPress:
          {
            pWaitResults->eventType = EVT_MOUSE;
            QMouseEvent* _pEvent = (QMouseEvent*)pEvent;
            if(_pEvent->button()==Qt::LeftButton)
              pWaitResults->eventData = WME_LBTNCLICK;
            if(_pEvent->button()==Qt::RightButton)
              pWaitResults->eventData = WME_RBTNCLICK;
            break;
          }
        case QEvent::MouseButtonDblClick:
          {
            pWaitResults->eventType = EVT_MOUSE;
            QMouseEvent* _pEvent = (QMouseEvent*)pEvent;
            if(_pEvent->button()==Qt::LeftButton)
              pWaitResults->eventData = WME_LBTNDBLCLICK;
            if(_pEvent->button()==Qt::RightButton)
              pWaitResults->eventData = WME_RBTNDBLCLICK;
            break;
          }
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
          {
            pWaitResults->eventType = EVT_KEYBOARD;
            QKeyEvent* _pEvent = evf.getKeyEvent();
            pWaitResults->eventData = _pEvent->nativeVirtualKey();
            break;
          }
        case QEvent::Timer:
        default:
           if(evf.getFiredEvent()!="")   // событие пользователя
            {
              pWaitResults->eventType = EVT_USER;
              QString sId = evf.getFiredEvent();
              pWaitResults->eventData = 0;
              for(int nId=0; nId<QS_MAX_EVENTS; nId++)
                if(sId == pWaitOptions->sUserEventIds[nId])
                  pWaitResults->eventData = (QHandle)pWaitOptions->sUserEventIds[nId];
              evf.setFiredEvent("");
              break;
            }
            else                          // событие таймера
            {
              pWaitResults->eventType = EVT_TIMER;
              pWaitResults->eventData = 0;
              break;
            }
        }
      }
      return Exit_WaitUserInput(QERR_NO_ERROR, &evf, timer);
    }
  }

  return Exit_WaitUserInput(QERR_NO_ERROR, &evf, timer);
}

QSNAP_API QError WaitUserInput
  (
  QHandle       hView,              // [in] хэндл окна, может быть NULL, тогда события
  WaitUserFlags waitFlags,          // [in] тип события (mouse, keyboard)
  WaitOptions*  pWaitOptions,       // [in] параметры ожидания (временной интервал и проч.)
  WaitResults*  pWaitResults        // [out] если указатель не нулевой, в WaitResults прописывается детальная информация о событии
  )
{
  QError qerr = QERR_NO_ERROR;
  auto waitUserInpRun = [&qerr, &hView, &waitFlags, &pWaitOptions, &pWaitResults]() -> void { 
    qerr = impl_WaitUserInput(hView, waitFlags, pWaitOptions, pWaitResults); 
  };
  executeCommand(waitUserInpRun);
  return qerr;
}

// добавление элемента управления
QSNAP_API QHandle impl_AddControl         // [ret] хэндл элемента управления
(
  QHandle         hView,            // [in]  хэндл панели управления
  QSnp::CtrlInfo* pControlInfo      // [in]  свойства элемента управления
)
{
  if(hView==QHANDLE_NULL)
    return QHANDLE_INVALID;

  QSnpToolbarView* pView = (QSnpToolbarView*)hView;
  QSnpToolBar* pToolbar = (QSnpToolBar*)pView->getWidget();
  if(!pToolbar)
    return QHANDLE_INVALID;

  QSnpInstance* pInstance = pView->getInstance();
  if(!pInstance)
    return QHANDLE_INVALID;

  QSnpEvent* pEvent=(QSnpEvent*)pInstance->getEvent(pControlInfo->szId);
  if(!pEvent)
    return QHANDLE_INVALID;

  QAction* pAction = pToolbar->addAction(pControlInfo->szCaption);
  pAction->setToolTip(pControlInfo->szDescription);
  pAction->connect(pAction, SIGNAL(triggered()), pToolbar, SLOT(onAction()) );

  // установка хэндла события в pAction
  QVariant data = pEvent->handle();
  pAction->setData(data);

  return (QHandle)pAction;
}

// добавление элемента управления
QSNAP_API QHandle AddControl         // [ret] хэндл элемента управления
  (
  QHandle         hView,            // [in]  хэндл панели управления
  QSnp::CtrlInfo* pControlInfo      // [in]  свойства элемента управления
  )
{
  QHandle hControl = QHANDLE_INVALID;
  auto cmdAddControl = [&]()
  {
    hControl = impl_AddControl(hView, pControlInfo); 
  };
  executeCommand(cmdAddControl);
  return hControl;
}

// добавление элемента управления
QSNAP_API QHandle impl_RemoveControl         // [ret] хэндл элемента управления
(
  QHandle         hControl          // [in]  хэндл элемента управления
)
{  
  return QERR_NOTIMPLEMENTED;
}

// удаление элемента управления
QSNAP_API QError RemoveControl
(
  QHandle         hControl          // [in]  хэндл элемента управления
)
{
  QError qerr = QERR_NO_ERROR;

  auto cmdRemoveControl = [&]()
  {
    qerr = impl_RemoveControl(hControl); 
  };
  executeCommand(cmdRemoveControl);

  return hControl;
}

QSNAP_API QHandle LoadExternalWidgetDll(const char* dllname)
{
  QHandle hDLib = QSpxLoadSpecialLibrary(dllname);
  return hDLib;
}

QSNAP_API QHandle CreateCustomWidget(QHandle hDll)
{
  if (hDll == QHANDLE_INVALID)
    return QHANDLE_INVALID;

  QHandle hLibrary = (QHandle)hDll;

  QW_DEF_TYPE(CreateWidget)();
  QW_DEF_FUNC(CreateWidget);
  QW_INIT(CreateWidget);

  QHandle hWidget = QHANDLE_INVALID;

  auto cmdCreateWidget = [&]()
  { 
    hWidget = (QHandle)QW_CALL(CreateWidget)();
  };
  executeCommand(cmdCreateWidget);

  return hWidget;
}

QSNAP_API bool GiveDataToWidget(QHandle hDll, QHandle pWidget, QHandle pFrame)
{
  if (hDll == QHANDLE_INVALID)
    return QHANDLE_INVALID;

  bool bRes = false;
  QHandle hLibrary = hDll;

  QW_DEF_TYPE(ReadInputData)(void* pWidget, void* pFrame);
  QW_DEF_FUNC(ReadInputData);

  QW_INIT(ReadInputData);

  auto cmdReadInputData = [&]()
  { 
    bRes = (QHandle)QW_CALL(ReadInputData)((void*)pWidget, (void*)pFrame);
  };
  executeCommand(cmdReadInputData);

  return bRes;
}

QSNAP_API QError CloseWidget(QHandle hDll, QHandle pWidget)
{
  if (hDll == QHANDLE_INVALID)
    return QHANDLE_INVALID;

  QError qerr = QERR_NO_ERROR;
  QHandle hLibrary = (QHandle)hDll;

  QW_DEF_TYPE(CloseWidget)(void* pWidget);
  QW_DEF_FUNC(CloseWidget);

  QW_INIT(CloseWidget);

  auto cmdCloseWidget = [&]()
  { 
    qerr = (QHandle)QW_CALL(CloseWidget)((void*)pWidget);
  };
  executeCommand(cmdCloseWidget);

  qerr = QSpxFreeLibrary(hLibrary);

  return qerr;
}
}; // namespace QSnp