#ifndef __DEBUG_H_
#define __DEBUG_H_


#define DEBUG 1

#ifdef DEBUG
#define dprint(...); printf(__VA_ARGS__);
#define crashprint(); dprint("\nDid we crash here? LINE: %d FILE: %s\n\n", __LINE__, __FILE__);
#else
#define dprint(...);
#endif


#endif //__DEBUG_H_
