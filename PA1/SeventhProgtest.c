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
TITEM            * sortList     ( TITEM           * l,
                                  int               ascending )
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
                if(strcmp(efir -> m_Name, sklo -> m_Name)<0)
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
                if(strcmp(efir -> m_Name, sklo -> m_Name)>0)
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
    l = newItem ( "PA1", l );
    l = newItem ( "PA2", l );
    l = newItem ( "UOS", l );
    l = newItem ( "LIN", l );
    l = newItem ( "AG1", l );
    assert ( l
             && ! strcmp ( l -> m_Name, "AG1" ) );
    assert ( l -> m_Next
             && ! strcmp ( l -> m_Next -> m_Name, "LIN" ) );
    assert ( l -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Name, "UOS" ) );
    assert ( l -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Name, "PA2" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "PA1" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
    l = sortList ( l, 1 );
    assert ( l
             && ! strcmp ( l -> m_Name, "AG1" ) );
    assert ( l -> m_Next
             && ! strcmp ( l -> m_Next -> m_Name, "LIN" ) );
    assert ( l -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Name, "PA1" ) );
    assert ( l -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Name, "PA2" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "UOS" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
    l = newItem ( "programming and algorithmic I", l );
    l = newItem ( "AAG", l );
    assert ( l
             && ! strcmp ( l -> m_Name, "AAG" ) );
    assert ( l -> m_Next
             && ! strcmp ( l -> m_Next -> m_Name, "programming and algorithmic I" ) );
    assert ( l -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Name, "AG1" ) );
    assert ( l -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Name, "LIN" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "PA1" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "PA2" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "UOS" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
    l = sortList ( l, 0 );
    assert ( l
             && ! strcmp ( l -> m_Name, "programming and algorithmic I" ) );
    assert ( l -> m_Next
             && ! strcmp ( l -> m_Next -> m_Name, "UOS" ) );
    assert ( l -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Name, "PA2" ) );
    assert ( l -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Name, "PA1" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "LIN" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "AG1" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "AAG" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
    freeList ( l );
    l = NULL;
    strncpy ( tmp, "PA1", sizeof ( tmp ) - 1 );
    tmp[sizeof ( tmp ) - 1 ] = '\0';
    l = newItem ( tmp, l );
    strncpy ( tmp, "PA2", sizeof ( tmp ) - 1 );
    tmp[sizeof ( tmp ) - 1 ] = '\0';
    l = newItem ( tmp, l );
    strncpy ( tmp, "UOS", sizeof ( tmp ) - 1 );
    tmp[sizeof ( tmp ) - 1 ] = '\0';
    l = newItem ( tmp, l );
    strncpy ( tmp, "LIN", sizeof ( tmp ) - 1 );
    tmp[sizeof ( tmp ) - 1 ] = '\0';
    l = newItem ( tmp, l );
    strncpy ( tmp, "AG1", sizeof ( tmp ) - 1 );
    tmp[sizeof ( tmp ) - 1 ] = '\0';
    l = newItem ( tmp, l );
    assert ( l
             && ! strcmp ( l -> m_Name, "AG1" ) );
    assert ( l -> m_Next
             && ! strcmp ( l -> m_Next -> m_Name, "LIN" ) );
    assert ( l -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Name, "UOS" ) );
    assert ( l -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Name, "PA2" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "PA1" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
    l = sortList ( l, 1 );
    assert ( l
             && ! strcmp ( l -> m_Name, "AG1" ) );
    assert ( l -> m_Next
             && ! strcmp ( l -> m_Next -> m_Name, "LIN" ) );
    assert ( l -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Name, "PA1" ) );
    assert ( l -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Name, "PA2" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "UOS" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
    strncpy ( tmp, "programming and algorithmic I", sizeof ( tmp ) - 1 );
    tmp[sizeof ( tmp ) - 1 ] = '\0';
    l = newItem ( tmp, l );
    strncpy ( tmp, "AAG", sizeof ( tmp ) - 1 );
    tmp[sizeof ( tmp ) - 1 ] = '\0';
    l = newItem ( tmp, l );
    assert ( l
             && ! strcmp ( l -> m_Name, "AAG" ) );
    assert ( l -> m_Next
             && ! strcmp ( l -> m_Next -> m_Name, "programming and algorithmic I" ) );
    assert ( l -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Name, "AG1" ) );
    assert ( l -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Name, "LIN" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "PA1" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "PA2" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "UOS" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
    l = sortList ( l, 0 );
    assert ( l
             && ! strcmp ( l -> m_Name, "programming and algorithmic I" ) );
    assert ( l -> m_Next
             && ! strcmp ( l -> m_Next -> m_Name, "UOS" ) );
    assert ( l -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Name, "PA2" ) );
    assert ( l -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Name, "PA1" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "LIN" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "AG1" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next
             && ! strcmp ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Name, "AAG" ) );
    assert ( l -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next -> m_Next == NULL );
    freeList ( l );
    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
