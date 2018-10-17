/* -*- c++ -*- */

#define MSGUTIL_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "msgutil_swig_doc.i"

%{
#include "msgutil/msgsink.h"
#include "msgutil/msgsrc.h"
%}


%include "msgutil/msgsink.h"
GR_SWIG_BLOCK_MAGIC2(msgutil, msgsink);
%include "msgutil/msgsrc.h"
GR_SWIG_BLOCK_MAGIC2(msgutil, msgsrc);
