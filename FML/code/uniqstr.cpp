#include "stdafx.h"
#include "medialibrary.h"

// little helper class for the arrays we maintain
/*
class CUniqueStringArray : public CStringArray
{
public:
	int Add( CString newElement);
};  
*/


int CUniqueStringArray::Add(CString newElement)
{
	int ub = GetUpperBound();
	int i = 0;

	// unfortunately the lists are all sorted by index, so this linear search needs to be used
	// on the unsorted text entries.
	while ((i <= ub)  && (GetAt(i).CompareNoCase(newElement) != 0)) i++;

	// if we don't already have this, then add it
	if ((ub == -1) || (i > ub) )
	{
		return CStringArray::Add(newElement);
	}
	else
	{
	// else return the index to the one we've already got
		return i;
	}
}

void CUniqueStringArray::Serialize(CArchive &ar)
{
	// yes, it is this easy to read or write this chunk of data to disk
	CStringArray::Serialize(ar);
}

