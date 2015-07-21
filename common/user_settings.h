#ifndef CUSTOM_H_
#define CUSTOM_H_

#define ACE_KBE_HAS_VERSIONED_NAMESPACE  1
#include "ace/Log_Msg.h"
#include "ace/Log_Record.h"
#include <fstream>

#define ACE_NTRACE 0
#include "ace/mytrace.h"

#define ENABLE_WATCHERS

// @config.h file in ace
// output inpout will not only align the buffer start osition, 
// but also the bytes stored in the buffer
// and so we have to setup this macro to stop automatic 
// alignment including start positionalignment and bytes alignment
// #define ACE_LACKS_CDR_ALIGNMENT

#endif