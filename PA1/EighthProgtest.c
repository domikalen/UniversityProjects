#ifndef __PROGTEST__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct TItem
{
    struct TItem             * m_Next;
    char                     * m_Name;
    char                       m_Secret[24];
} TITEM;

int cmpName ( const TITEM * a,
              const TITEM * b )
{
    return strcmp ( a -> m_Name, b -> m_Name );
}

int cmpNameInsensitive ( const TITEM * a,
                         const TITEM * b )
{
    return strcasecmp ( a -> m_Name, b -> m_Name );
}

int cmpNameLen ( const TITEM * a,
                 const TITEM * b )
{
    size_t la = strlen ( a -> m_Name ), lb = strlen ( b -> m_Name );
    return ( lb < la ) - ( la < lb );
}
#endif /* __PROGTEST__ */

TITEM            * newItem      ( const char      * name,
                                  TITEM           * next )
{
    TITEM * suka = (TITEM *) malloc (sizeof ( *suka ));
    suka -> m_Name = (char *) malloc(sizeof ( name )* ( strlen(name)+1));
    strcpy(suka -> m_Name, name);
    suka -> m_Secret[0] = '\0';
    suka -> m_Next = next;
    return suka;
}

TITEM * swich(TITEM * sus1, TITEM * sus2)
{
    TITEM* tmp = sus2 -> m_Next;
    sus2 -> m_Next =sus1;
    sus1 -> m_Next = tmp;
    return sus2;
}
TITEM            * sortListCmp  ( TITEM           * l,
                                  int               ascending,
                                  int            (* cmpFn) ( const TITEM *, const TITEM *) )
{
    TITEM ** tmp = &l;
    TITEM * ret = l;
    int count = 0;
    while(ret!=NULL)
    {
        count++;
        ret = ret ->m_Next;
    }

    if(ascending==0)
    {
        for(int i = 0; i <= count; i++)
        {

            tmp = &l;
            int lol = 0;
            for(int j = 0; j < count - i - 1; j++)
            {
                TITEM* efir = *tmp;
                TITEM* sklo = efir -> m_Next;
                if(cmpFn(efir, sklo)<0)
                {
                    *tmp = swich(efir, sklo);
                    lol = 1;

                }
                tmp = &(*tmp) -> m_Next;
            }
            if(lol==0)
                break;

        }
    }
    else if(ascending==1)
    {
        for(int i = 0; i <= count; i++)
        {

            tmp = &l;
            int lol = 0;
            for(int j = 0; j < count - i - 1; j++)
            {
                TITEM* efir = *tmp;
                TITEM* sklo = efir -> m_Next;
                if(cmpFn(efir, sklo)>0)
                {
                    *tmp = swich(efir, sklo);
                    lol = 1;

                }
                tmp = &(*tmp) -> m_Next;
            }
            if(lol==0)
                break;

        }
    }
    ret = l;
    return ret;
}
void               freeList     ( TITEM           * src )
{
    while (src)
    {
        free(src ->m_Name);
        TITEM * tmp = src -> m_Next;
        free (src);
        src = tmp;
    }
}

#ifndef __PROGTEST__
int main ( int argc, char * argv [] )
{
    TITEM * l;
    char tmp[50];

    assert ( sizeof ( TITEM ) == sizeof ( TITEM * ) + sizeof ( char * ) + 24 * sizeof ( char ) );
    l = NULL;
    l = newItem ( "BI-PA1", l );
    l = newItem ( "BIE-PA2", l );
    l = newItem ( "NI-PAR", l );
    l = newItem ( "lin", l );
    l = newItem ( "AG1", l );
    assert ( l
             && ! strcmp ( l -> m_Name, "AG1" ) );
    assert ( l -> m_Next
             && ! strcmp ( l -> m_Next -> m_Name, "lin" ) );
    assert ( l -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Name, "NI-PAR" ) );
    assert ( l -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Name, "BIE-PA2" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "BI-PA1" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
    l = sortListCmp ( l, 1, cmpName );
    assert ( l
             && ! strcmp ( l -> m_Name, "AG1" ) );
    assert ( l -> m_Next
             && ! strcmp ( l -> m_Next -> m_Name, "BI-PA1" ) );
    assert ( l -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Name, "BIE-PA2" ) );
    assert ( l -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Name, "NI-PAR" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "lin" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
    l = sortListCmp ( l, 1, cmpNameInsensitive );
    assert ( l
             && ! strcmp ( l -> m_Name, "AG1" ) );
    assert ( l -> m_Next
             && ! strcmp ( l -> m_Next -> m_Name, "BI-PA1" ) );
    assert ( l -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Name, "BIE-PA2" ) );
    assert ( l -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Name, "lin" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "NI-PAR" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
    l = sortListCmp ( l, 1, cmpNameLen );
    assert ( l
             && ! strcmp ( l -> m_Name, "AG1" ) );
    assert ( l -> m_Next
             && ! strcmp ( l -> m_Next -> m_Name, "lin" ) );
    assert ( l -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Name, "BI-PA1" ) );
    assert ( l -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Name, "NI-PAR" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "BIE-PA2" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
    l = newItem ( "AAG.3", l );
    assert ( l
             && ! strcmp ( l -> m_Name, "AAG.3" ) );
    assert ( l -> m_Next
             && ! strcmp ( l -> m_Next -> m_Name, "AG1" ) );
    assert ( l -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Name, "lin" ) );
    assert ( l -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Name, "BI-PA1" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "NI-PAR" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "BIE-PA2" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
    l = sortListCmp ( l, 0, cmpNameLen );
    assert ( l
             && ! strcmp ( l -> m_Name, "BIE-PA2" ) );
    assert ( l -> m_Next
             && ! strcmp ( l -> m_Next -> m_Name, "BI-PA1" ) );
    assert ( l -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Name, "NI-PAR" ) );
    assert ( l -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Name, "AAG.3" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "AG1" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "lin" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
    freeList ( l );
    l = NULL;
    strncpy ( tmp, "BI-PA1", sizeof ( tmp ) - 1 );
    tmp[sizeof ( tmp ) - 1 ] = '\0';
    l = newItem ( tmp, l );
    strncpy ( tmp, "BIE-PA2", sizeof ( tmp ) - 1 );
    tmp[sizeof ( tmp ) - 1 ] = '\0';
    l = newItem ( tmp, l );
    strncpy ( tmp, "NI-PAR", sizeof ( tmp ) - 1 );
    tmp[sizeof ( tmp ) - 1 ] = '\0';
    l = newItem ( tmp, l );
    strncpy ( tmp, "lin", sizeof ( tmp ) - 1 );
    tmp[sizeof ( tmp ) - 1 ] = '\0';
    l = newItem ( tmp, l );
    strncpy ( tmp, "AG1", sizeof ( tmp ) - 1 );
    tmp[sizeof ( tmp ) - 1 ] = '\0';
    l = newItem ( tmp, l );
    assert ( l
             && ! strcmp ( l -> m_Name, "AG1" ) );
    assert ( l -> m_Next
             && ! strcmp ( l -> m_Next -> m_Name, "lin" ) );
    assert ( l -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Name, "NI-PAR" ) );
    assert ( l -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Name, "BIE-PA2" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "BI-PA1" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
    l = sortListCmp ( l, 1, cmpName );
    assert ( l
             && ! strcmp ( l -> m_Name, "AG1" ) );
    assert ( l -> m_Next
             && ! strcmp ( l -> m_Next -> m_Name, "BI-PA1" ) );
    assert ( l -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Name, "BIE-PA2" ) );
    assert ( l -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Name, "NI-PAR" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "lin" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
    l = sortListCmp ( l, 1, cmpNameInsensitive );
    assert ( l
             && ! strcmp ( l -> m_Name, "AG1" ) );
    assert ( l -> m_Next
             && ! strcmp ( l -> m_Next -> m_Name, "BI-PA1" ) );
    assert ( l -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Name, "BIE-PA2" ) );
    assert ( l -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Name, "lin" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "NI-PAR" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
    l = sortListCmp ( l, 1, cmpNameLen );
    assert ( l
             && ! strcmp ( l -> m_Name, "AG1" ) );
    assert ( l -> m_Next
             && ! strcmp ( l -> m_Next -> m_Name, "lin" ) );
    assert ( l -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Name, "BI-PA1" ) );
    assert ( l -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Name, "NI-PAR" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "BIE-PA2" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
    strncpy ( tmp, "AAG.3", sizeof ( tmp ) - 1 );
    tmp[sizeof ( tmp ) - 1 ] = '\0';
    l = newItem ( tmp, l );
    assert ( l
             && ! strcmp ( l -> m_Name, "AAG.3" ) );
    assert ( l -> m_Next
             && ! strcmp ( l -> m_Next -> m_Name, "AG1" ) );
    assert ( l -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Name, "lin" ) );
    assert ( l -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Name, "BI-PA1" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "NI-PAR" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "BIE-PA2" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
    l = sortListCmp ( l, 0, cmpNameLen );
    assert ( l
             && ! strcmp ( l -> m_Name, "BIE-PA2" ) );
    assert ( l -> m_Next
             && ! strcmp ( l -> m_Next -> m_Name, "BI-PA1" ) );
    assert ( l -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Name, "NI-PAR" ) );
    assert ( l -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Name, "AAG.3" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "AG1" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "lin" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
    freeList ( l );
    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
