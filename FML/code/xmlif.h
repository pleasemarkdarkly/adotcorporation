//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1995-2001.
//
//  File:       X M L I F . H
//
//  Contents:   Header file for interface to the XML parser
//
//----------------------------------------------------------------------------

typedef PVOID HELEMENT;
typedef PVOID HATTRIBUTE;

#define XMLPARSE_INPLACE    1

//---------- XML Interface functions ---------------------

// Parses an XML document; returns a handle to the root element.
DWORD     XMLParse(PCSTR pszInput, HELEMENT* hRoot);

// Frees the parse tree; hRoot is the handle returned by XMLParse.
VOID XMLFree(HELEMENT hRoot);           

// Obtain a child element; returns a handle to the element.
HELEMENT    XMLGetFirstChild(HELEMENT);

// Obtain a sibling element; returns a handle to the element. 
HELEMENT    XMLGetNextSibling(HELEMENT);

// Obtain the first attribute; obtains a handle to the attribute. 
HATTRIBUTE  XMLGetFirstAttribute(HELEMENT);

// Obtain the next attribute; obtains a handle to the attribute. 
HATTRIBUTE  XMLGetNextAttribute(HATTRIBUTE);

// NOTE:  All strings returned by the following XMLGet* functions 
// are owned by the XML DLL.  Use in place or make copies. 
// Do NOT free them!

// Functions to get names and values. 
PCSTR XMLGetElementName(HELEMENT);
PCSTR XMLGetElementNameSpace(HELEMENT);
PCSTR XMLGetAttributeName(HATTRIBUTE);
PCSTR XMLGetAttributeNameSpace(HATTRIBUTE);
PCSTR XMLGetElementValue(HELEMENT);
PCSTR XMLGetAttributeValue(HATTRIBUTE);


