///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       memdbg.cpp
//  Content:    D3DX memory debugging functions
//
///////////////////////////////////////////////////////////////////////////

#include "mviewpch.h"

#ifdef MEM_DEBUG

#include <stdlib.h>
#include <stdio.h>


static UINT32 TotalAllocated = 0;       // Total allocated
static UINT32 CurrentAllocation = 0;    // Current size of allocated memory
static UINT32 MaxHeld = 0;          // Maximum size held at any point in time
static UINT32 BlocksAllocated = 0;

struct SMemInfo
{
    SMemInfo    *pbNext;    
    SMemInfo    *pbPrev;    
    char        *szFilename;
    UINT32      ulLine;
    UINT32      ulAllocationSize;
    UINT32      ulBlockNo;
};

#define BLOCK_SIZE (sizeof(SMemInfo))

static SMemInfo *AllocationHead = NULL;

// User-defined operator new.
void *operator new( size_t stAllocateBlock )
{
    static bool fInOpNew = 0;    // Guard flag.
    UINT32 uiSize = 0;
    BYTE *NewBlock;
    SMemInfo *pmeminfo;

    if(!fInOpNew )
    {
        fInOpNew = 1;

        TotalAllocated += stAllocateBlock;

        CurrentAllocation += stAllocateBlock;

        if (CurrentAllocation > MaxHeld)
            MaxHeld = CurrentAllocation;

        uiSize = stAllocateBlock;

        fInOpNew = 0;
    }
    else
    {
        // set size to 0 on non loggable blocks
        uiSize = 0;
    }

    NewBlock = (BYTE *)malloc( stAllocateBlock + sizeof(SMemInfo));
    if (NewBlock == NULL)
    {
        return NULL;
    }
    else
    {
        pmeminfo = (SMemInfo *)NewBlock;
        pmeminfo->ulAllocationSize = uiSize;
        pmeminfo->szFilename = __FILE__;
        pmeminfo->ulLine = __LINE__;
        pmeminfo->pbNext = AllocationHead;
        if (AllocationHead != NULL)
            AllocationHead->pbPrev = (SMemInfo*)NewBlock;
        pmeminfo->pbPrev = NULL;

        static int BlocksTest = 0xffffffff;
        if (BlocksAllocated == BlocksTest)
        {
            BlocksAllocated = BlocksTest;
        }

        pmeminfo->ulBlockNo = BlocksAllocated;
        BlocksAllocated += 1;

        AllocationHead = (SMemInfo*)NewBlock;
        return NewBlock + sizeof(SMemInfo);
    }


}

// User-defined operator new. (with file and line numbers)
void *operator new(size_t stAllocateBlock, const UINT32 uiLineNumber, const char *szFilename)
{
    static bool fInOpNew = 0;    // Guard flag.
    UINT32 uiSize = 0;
    BYTE *NewBlock;
    SMemInfo *pmeminfo;

    if(!fInOpNew )
    {
        fInOpNew = 1;

        //printf("Memory Allocated at %s:%d\n", szFilename, uiLineNumber);

        TotalAllocated += stAllocateBlock;

        CurrentAllocation += stAllocateBlock;

        if (CurrentAllocation > MaxHeld)
            MaxHeld = CurrentAllocation;

        uiSize = stAllocateBlock;

        fInOpNew = 0;
    }
    else
    {
        // set size to 0 on non loggable blocks
        uiSize = 0;
    }

    NewBlock = (BYTE *)malloc(stAllocateBlock + sizeof(SMemInfo));
    if (NewBlock == NULL)
    {
        return NULL;
    }
    else
    {
        pmeminfo = (SMemInfo *)NewBlock;
        pmeminfo->ulAllocationSize = uiSize;
        pmeminfo->szFilename = (char*)szFilename;
        pmeminfo->ulLine = uiLineNumber;
        pmeminfo->pbNext = AllocationHead;
        if (AllocationHead != NULL)
            AllocationHead->pbPrev = (SMemInfo*)NewBlock;
        pmeminfo->pbPrev = NULL;
        pmeminfo->ulBlockNo = BlocksAllocated;
        BlocksAllocated += 1;

        AllocationHead = (SMemInfo*)NewBlock;
        return NewBlock + sizeof(SMemInfo);
    }


}



// User-defined operator delete.
void operator delete( void *pvMem )
{
    static fInOpDelete = 0;    // Guard flag.
    BYTE *pbMem = (BYTE *)pvMem;
    SMemInfo *pbNextElem, *pbPrevElem;

    if (pvMem == NULL)
        return;

    pbMem -= BLOCK_SIZE;    // move back to the start of the allocated block

    pbPrevElem = ((SMemInfo*)pbMem)->pbPrev;
    pbNextElem = ((SMemInfo*)pbMem)->pbNext;

    // if at the head, then remove the head element
    if (pbPrevElem == NULL)
    {
        if (pbNextElem != NULL)
        {
            pbNextElem->pbPrev = NULL;
        }

        AllocationHead = pbNextElem;
    }
    else
    {
        pbPrevElem->pbNext = pbNextElem;

        // NULL if the allocation is at the end of the list
        if (pbNextElem != NULL)
            pbNextElem->pbPrev = pbPrevElem;
    }

    if( !fInOpDelete )
    {
        fInOpDelete = 1;

        CurrentAllocation -= ((SMemInfo *)pbMem)->ulAllocationSize;

        fInOpDelete = 0;
    }

//  _ASSERT(_CrtIsMemoryBlock(pbMem), ((SMemInfo *)pbMem)->ulAllocationSize);
    free(pbMem);
}


//
// returns true if any memory was not freed
//
BOOL WINAPI D3DXDumpUnfreedMemoryInfo()
{
    SMemInfo *pbCurElem;
    char *szFilename;
    UINT32 uiLineNumber;
    UINT32 uiSize;
    UINT32 ulBlockNo;
    char szBuf[512];

    // if no elements on list, then no leaks
    if (AllocationHead == NULL)
    {
        return FALSE;
    }

    MessageBox(NULL, "Look in debug output window for more info", "Memory Leaks!", MB_OK);

    pbCurElem = AllocationHead;

    while (pbCurElem != NULL)
    {
        szFilename = pbCurElem->szFilename;
        uiLineNumber =  pbCurElem->ulLine;
        uiSize = pbCurElem->ulAllocationSize;      
        ulBlockNo = pbCurElem->ulBlockNo;

        sprintf(szBuf, "Memory Leak(%d):  %d bytes leaked at %s: %d\n", ulBlockNo, uiSize, szFilename, uiLineNumber);
        OutputDebugString(szBuf);

        pbCurElem = ((SMemInfo*)pbCurElem)->pbNext;
    }

    return TRUE;
}

#else

BOOL WINAPI D3DXDumpUnfreedMemoryInfo()
{
    return FALSE;
}

#endif
