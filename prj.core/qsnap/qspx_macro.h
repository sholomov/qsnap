/**
  \file   qspx_macro.h
  \brief  QSnap proxy classes macro helpers
  \author Sholomov D.
  \date   05.06.2013
*/
#pragma once
#ifndef QSPX_MACRO_H
#define QSPX_MACRO_H

#include <qsnap/qsnap.h>
#include <assert.h>

///////////////////////////////////////////////////////////////////////
////  Macro helpers section

// корректно загружен ли и проинициализирован QSnap
#define QW_READY( pinstance ) ((pinstance)!=0 && (pinstance)->library()!=0 && (pinstance)->handle()!=0)

// определение типа функции
#define QW_DEF_TYPE( fname ) typedef QHandle (*t##fname)

// определение типа функции c типом возвращаемого значения
#define QW_DEF_TYPE_EX( fname, rettype ) typedef rettype (*t##fname)

// определение статического указателя на функцию
#define QW_DEF_FUNC( fname ) t##fname p##fname;

// получение указателя на функцию
#define QW_INIT( fname )  p##fname = (t##fname)QSpxGetFunction(hLibrary, #fname); //assert(p##fname);

// обнуление указателя на функцию
#define QW_FREE( fname ) p##fname = QHANDLE_NULL;

// получение указателя на функцию
#define QW_INIT_NULL( fname )  p##fname(nullptr)

// вызов функции по указателю
#define QW_CALL( fname ) !(p##fname) ? QERR_NO_ERROR : p##fname

// вызов функции по указателю
#define QW_PCALL( cl, fname ) !(cl->p##fname) ? QERR_NO_ERROR : cl->p##fname

// вызов функции, с возвращением NULL
#define QW_NULLCALL( fname ) !(p##fname) ? QHANDLE_NULL : p##fname

// вызов функции только в случае не нулевого указателя (safe call)
#define QW_SAFECALL( cl ) if(cl) cl

// вызов функции возвращающей значение только в случае не нулевого указателя (safe call)
#define QW_RETCALL( cl ) !cl ? NULL : cl

// вызов функции возвращающей значение только в случае не нулевого указателя (safe call)
#define QW_BOOLCALL( cl ) cl && cl

// удаление вызов функции возвращающей значение только в случае не нулевого указателя (safe call)
#define QW_RELEASE( view ) { if (view) delete view; view = 0; }

#endif // QSPX_MACRO_H
