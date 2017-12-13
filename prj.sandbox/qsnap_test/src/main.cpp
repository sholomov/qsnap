#include <opencv2/core/core.hpp>
#include <direct.h>

#include <qsnap/qsnapx.h>

using namespace std;
using namespace cv;
using namespace QSnp;

std::string moduleDirPath();

int main(int argc, char *argv[])
{
  static QSpxInstance snpInstance;
  snpInstance.initialize(false);

  // создание окна SnapTreeView
  QSpxTreeView* pTreeView = static_cast<QSpxTreeView*>(snpInstance.createView(QSnp::VT_SNAPTREE_VIEW,  "SnapTreeView"));
  QW_SAFECALL(pTreeView)->setTitle("Hello!!");
  QSpxNode* pRoot = QW_RETCALL(pTreeView)->snapTreeRoot();
  QSpxNode* pNode1 = QW_RETCALL(pRoot)->addChildNode("Node1", "Node1");
  QSpxNode* pNode2 = QW_RETCALL(pRoot)->addChildNode("Node2", "Node2");
  QSpxNode* pNode3 = QW_RETCALL(pNode1)->addChildNode("Node3", "Node3");

  // загрузка свойств вершин
  QW_SAFECALL(pNode1)->loadProperties();

  // создание окна TextView
  QSpxTextView* pTextView = static_cast<QSpxTextView*>(snpInstance.createView(QSnp::VT_TEXT_VIEW,  "LogView"));
  QW_SAFECALL(pTextView)->logLn("Hello %s","!!!!!");
  QW_SAFECALL(pTextView)->setTitle("Hello my TextView!!!");

  // создание окна ImageView
  QSpxImageView* pImageView = static_cast<QSpxImageView*>(snpInstance.createView(QSnp::VT_IMAGE_VIEW,  "ImageView"));
  
  // Формирование пути к изображению
  string imagePath = string(moduleDirPath()) +"../data/qsnap/001.jpg";
  QW_SAFECALL(pImageView)->setImage(imagePath.c_str());

  // изменение заголовка окна
  QW_SAFECALL(pImageView)->setTitle("Hello!!!");

  // отрисовка геометрических фигур
  for(int nRect = 0; nRect<9; nRect++)
  {
    // прямоугольники
    SnpRect rc ={10, 10+60*nRect, 400, 50};
    int color =RGB(255-30*nRect, 20*nRect, 30*nRect); // цвет меняется от красного до темного
    QW_SAFECALL(pImageView)->drawRect(rc, nRect+1, color, LT_LINE, NULL);

    // линии
    cv::Point2i ptFrom(rc.x+rc.width-1, rc.y+rc.height-1);
    cv::Point2i ptTo(600, 10+30*nRect);
    QW_SAFECALL(pImageView)->drawLine(ptFrom.x, ptFrom.y, ptTo.x, ptTo.y, nRect+1, color, LT_LINE, NULL);

    // эллипсы
    SnpRect rce = {ptTo.x, ptTo.y-10, 40, 20};
    QW_SAFECALL(pImageView)->drawEllipse(rce, nRect+1, color, LT_LINE, NULL);

    // точки
    QW_SAFECALL(pImageView)->drawPoint(ptTo.x+15, ptTo.y, nRect+1, color, NULL);
    QW_SAFECALL(pImageView)->drawPoint(ptTo.x+25, ptTo.y, nRect+1, color, NULL);

  }
  QW_SAFECALL(pImageView)->update();
  

  // ожидание события пользователя
  WaitOptions waitOptions = {3000};
  WaitResults waitResults = {};
  snpInstance.waitUserInput(WF_ANY, &waitOptions, &waitResults);
  QW_SAFECALL(pTextView)->logLn("While waiting signal \'%d\' received (%d,%d)", waitResults.eventType, waitResults.eventData, waitResults.nModifiers);

  // изменение размеров окна
  SnpRect rcOld = {0};
  if(pImageView)
    rcOld = pImageView->rect();// sleep(500);
  const int delta = 40;
  SnpRect rcNew = {
    rcOld.x+delta,
    rcOld.y+delta, 
    rcOld.width-delta*2, 
    rcOld.height-delta*2
  };
  QW_SAFECALL(pImageView)->setRect(rcNew);// sleep(500);
  QW_SAFECALL(pImageView)->setRect(rcOld);// sleep(1000);

  // ожидание нажатия на пробел
  waitOptions.nTimeInterval = 0;
  waitOptions.nKbdEventFilter = WKF_KEY;
  waitOptions.nKbdFilterData = ' ';  // код пробела
  snpInstance.waitUserInput(WF_KEYBOARD, &waitOptions, &waitResults);

  // удаление объектов
  delete pImageView;
  delete pTextView;
  delete pTreeView;

  snpInstance.terminate();

  return 1;
}

std::string moduleDirPath()
{
  char libraryPathBuf[256] = {0};

#ifdef _MSC_VER
  HMODULE hModule = 0;
  BOOL ret = GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)&__FUNCTION__, &hModule);
  DWORD nLen = GetModuleFileNameA(hModule, libraryPathBuf, sizeof(libraryPathBuf));
  if (nLen == sizeof(libraryPathBuf))
    _getcwd(libraryPathBuf, sizeof(libraryPathBuf));

  char drive[256] = {0}, directory[1024] = {0};
  _splitpath(libraryPathBuf, drive, directory, 0, 0);
  _makepath(libraryPathBuf, drive, directory, 0, 0);
#elif __linux__
  if (getcwd(libraryPathBuf, sizeof(libraryPathBuf)) == NULL)
      return std::string();
#endif

  std::string libraryPath(libraryPathBuf);
  const char lastChar = libraryPath[libraryPath.length() - 1];
  if (lastChar != '\\' && lastChar != '/')
    libraryPath.append("/");

  return libraryPath;
}
