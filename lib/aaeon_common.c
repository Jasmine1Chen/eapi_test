#include <stdio.h>
#include <sys/stat.h>

#include <common.h>
#include <DbgChk.h>

int direxist(char* path)
{
    struct stat sb;

    if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode))
    {
   
        DBG("YES\n");
        
	return 1;
    }
    else
    {
    
        DBG("NO\n");
       
	return 0;
    }
}
