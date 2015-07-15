#ifndef _SA_ASSERT_H_
#define _SA_ASSERT_H_

#include <assert.h>
#include <stdio.h>

#ifndef SA_ASSERT
#define SA_ASSERT(f, l, m, x) do{				\
	assert(x);									\
	}while(0)
#endif

#ifndef SA_STR
#define SA_STR(x) (#x)
#endif

#ifndef SA_STR_CON
#define SA_STR_CON(x,y,z)   (SA_STR(x)##SA_STR(y)##SA_STR(z)) 
#endif

//printf("%s:%d:%s\n", f, l, m);        
#ifndef SA_ASSERT_RETURN
#define SA_ASSERT_RETURN(f, l, m, x,r) do{		\
	assert(x);							\
	return r;							\
	}while(0)
#endif




#endif
