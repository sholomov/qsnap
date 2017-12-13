// инициализация библиотеки
QSNAP_API QError Initialize
(
);

QSNAP_API QError Terminate
(
);

// работа с вьюшками
QSNAP_API QHandle CreateView
(
  QSnp::ViewType eViewType 
);

QSNAP_API QError DestroyView
(
  QHandle hView
);

QSNAP_API QError ShowView
(
  QHandle hView,
  bool    bShow
);

// работа с конфигурациями вьюшек
QSNAP_API QError LoadViewConfig
(
  const char* szConfigName
);

QSNAP_API QError SaveCurrentViewConfig
(
  const char* szConfigName
);

QSNAP_API QError DestroyAllViews
(
);

QSNAP_API QError ShowAllViews
(
 bool bShow
);

// работа с деревом снэпа
QSNAP_API QSnapNode GetSnapTreeRoot
(
  const char* szConfigName
);

QSNAP_API QSnapNode AddNode
(
  QSnapNode       hParentNode,
  QSnp::NodeInfo  nodeInfo
);

QSNAP_API QError RemoveNode
(
  QSnapNode hNode
);
