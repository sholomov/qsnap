/**
  \file   qsnp_wrap.h
  \brief  C++ wrapper classes for QSnap library
  \author Sholomov D.
  \date   05.06.2013
*/
#include <qsnap/qsnap.h>
#include <assert.h>

///////////////////////////////////////////////////////////////////////
////  Macro helpers section

// определение типа функции
#define QW_DEF_TYPE( fname ) typedef QHandle (*t##fname)

// определение статического указателя на функцию
#define QW_DEF_FUNC( fname ) t##fname p##fname;

// получение указателя на функцию
#define QW_INIT( fname ) QSnwInstance::p##fname = (t##fname)QSnwGetFunction(QSnwInstance::hLibrary, "fname"); assert(p##fname);

// обнуление указателя на функцию
#define QW_FREE( fname ) p##fname = QHANDLE_NULL;

// вызов функции по указателю
#define QW_CALL( fname ) QSnwInstance::p##fname

///////////////////////////////////////////////////////////////////////
////  QSnwInstance class

class QSnwInstance
{
public:
  // конструктор
  QSnwInstance() : hInstance(0), hLibrary(0) {}
  
  // деструктор
  virtual ~QSnwInstance() { terminate(); } 

  // инициализация инстанса снэпа
  QError initialize();

  // деинициализация инстанса снэпа
  QError terminate();

protected:
  QHandle hInstance;

protected:
  QHandle hLibrary;   /// хэндл подгружаемой библиотеки

  // указатели на функции
  QW_DEF_TYPE(Initialize)(); 
  QW_DEF_FUNC(Initialize);

  QW_DEF_TYPE(Terminate)(QHandle hInstance);
  QW_DEF_FUNC(Terminate);

  QW_DEF_TYPE(CreateView)(QHandle hInstance, QSnp::ViewType eViewType, const char* sId);
  QW_DEF_FUNC(CreateView);

 };

inline QHandle  QSnwLoadLibrary();
inline QError   QSnwFreeLibrary(QHandle hLibrary);
inline QHandle  QSnwGetFunction(QHandle hLibrary, const char*   pFuncName);

// инициализация инстанса снэпа
inline QError QSnwInstance::initialize()
{
  using namespace QSnp;

  if(hInstance)
    return QERR_NO_ERROR;

  // загрузка библиотеки
  if(hLibrary==QHANDLE_NULL)
    hLibrary = QSnwLoadLibrary();

  // инициализация динамически подгружаемых функций
  QW_INIT(Initialize);
  QW_INIT(Terminate);

  // инициализация экземпляра снепа
  hInstance = QW_CALL(Initialize)();

  return hInstance != QHANDLE_NULL ? QERR_NO_ERROR : QERR_ERROR;
}

// деинициализация инстанса снэпа
inline QError QSnwInstance::terminate()
{
  using namespace QSnp;

  if(hInstance)
  {
    QW_CALL(Terminate)(hInstance);
    hInstance = NULL;
  }
  if(hLibrary)
  {
    QSnwFreeLibrary(hLibrary);
    hLibrary = QHANDLE_NULL;
  }
  return QERR_NO_ERROR;
}

////////////////////////////////////////////////////////
//////    служебные функции

#include <string.h>

#ifdef WIN32 // OS Windows implementation
#include <windows.h>

#ifdef UNICODE
  #define _strcat wcscat
  #define _strlen wcslen
#else
  #define _strcat strcat
  #define _strlen strlen
#endif

inline QHandle QSnwLoadLibrary()
{
  using namespace std;
  TCHAR szFileName[260]={0};
  ::GetModuleFileName(0, szFileName,sizeof(szFileName)-1);
  bool bSlashFound=false;
  int len = _strlen(szFileName);
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

// выгрузка библиотеки снепа
inline QError QSnwFreeLibrary
(
  QHandle       hLibrary            // [in] хэндл библиотеки
)
{
  if(hLibrary==QHANDLE_NULL)
    return QERR_NO_ERROR;
  return ::FreeLibrary((HMODULE)hLibrary);
}

// получение адреса функции
inline QHandle QSnwGetFunction
(
  QHandle       hLibrary,           // [in] хэндл библиотеки
  const char*   pFuncName           // [in] имя функции
)
{
  return 0;
}

#endif // _WINDOWS_
