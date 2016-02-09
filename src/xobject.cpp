#include "xobject.hpp"

namespace pparam
{

// XObjectStatus Implementation

const string XObjectStatus::typeString[XObjectStatus::MAX] = {
	"loaded",
	"adding",
	"normal",
	"modifying",
	"deleting",
	"deleted",
	"scanning",
	"querying",
	"reloading",
	"printing",
	};

const XObjectStatus::StatusTransition 
	XObjectStatus::transitionTable[MAX /* from */][MAX /* to */] = {
		/* 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, */
		{  N, D, N, N, D, N, N, D, D, D},	/* 0 - LOADED */
		{  D, W, D, W, W, N, W, W, W, W},	/* 1 - ADDING */
		{  N, N, N, D, D, N, D, D, N, D},	/* 2 - NORMAL */
		{  N, N, D, W, W, N, W, W, N, W},	/* 3 - MODIFYING */
		{  N, N, D, W, W, D, W, W, N, W},	/* 4 - DELETING */
		{  N, N, N, N, N, N, N, N, N, D},	/* 5 - DELETED */
		{  N, N, D, W, W, N, W, W, N, W},	/* 6 - SCANNING */
		{  D, N, D, W, W, N, W, W, N, W},	/* 7 - QUERYING */
		{  D, W, N, N, N, N, N, N, W, W},	/* 8 - RELOADING */
		{  D, W, D, W, W, D, W, W, W, W},	/* 9 - PRINTING */
	};

} // namespace pparam
