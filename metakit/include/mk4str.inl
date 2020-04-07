//........................................................................................
//........................................................................................
//.. File Name: mk4str.inl
//.. Last Modified By: Donni Reitz-Pesek	donni@iobjects.com
//.. Modification date: 2/17/2000
//........................................................................................
//.. Copyright:(c) 1995-2000 Interactive Objects Inc.
//..	 All rights reserved. This code may not be redistributed in source or linkable
//.. 	 object form without the express written consent of Interactive Objects.
//.. Contact Information: www.iobjects.com
//........................................................................................
//........................................................................................
//  Copyright (C) 1996-2000 Jean-Claude Wippler.  All rights reserved.
//
//  This file contains inline definitions of the string package.
//      
//  Only very small members are selected for inline code.
//  See the class listing for a description of these functions.

/////////////////////////////////////////////////////////////////////////////
// c4_String

#if q4_MFC                      // Microsoft Foundation Classes
    
#elif q4_STD                    // STL and standard strings

d4_inline c4_String::c4_String ()
{
}

d4_inline c4_String::c4_String (char ch_, int nDup_)
    : string (nDup_, ch_)
{
}

d4_inline c4_String::c4_String (const char* str_)
    : string (str_)
{
}

d4_inline c4_String::c4_String (const void* ptr_, int len_)
    : string ((const char*) ptr_, len_)
{
}

d4_inline c4_String::c4_String (const d4_std::string& s_)
    : string (s_)
{
}

d4_inline c4_String::c4_String (const c4_String& s_)
    : string (s_)
{
}

d4_inline c4_String::~c4_String ()
{
}

d4_inline const c4_String& c4_String::operator= (const c4_String& s_)
{
    *(string*) this = s_;
    return *this;
}

d4_inline c4_String::operator const char* () const
{
    return c_str();
}

d4_inline char c4_String::operator[] (int i) const
{
    return at(i);
}
    
d4_inline c4_String operator+ (const c4_String& a_, const c4_String& b_)
{
    return (d4_std::string) a_ + (d4_std::string) b_; 
}

d4_inline c4_String operator+ (const c4_String& a_, const char* b_)
{
    return (d4_std::string) a_ + (d4_std::string) b_; 
}

d4_inline c4_String operator+ (const char* a_, const c4_String& b_)
{
    return (d4_std::string) a_ + (d4_std::string) b_; 
}

d4_inline const c4_String& c4_String::operator+= (const c4_String& s_)
{
    *(string*) this += s_;
    return *this;
}

d4_inline const c4_String& c4_String::operator+= (const char* s_)
{
    *(string*) this += s_;
    return *this;
}

d4_inline int c4_String::GetLength() const
{
    return length();
}

d4_inline bool c4_String::IsEmpty() const
{
    return empty();
}

d4_inline void c4_String::Empty()
{
    erase();
}

d4_inline c4_String c4_String::Left(int nCount_) const
{
    if (nCount_ > length())
        nCount_ = length();
    return substr(0, nCount_);
}

d4_inline c4_String c4_String::Right(int nCount_) const
{
    if (nCount_ > length())
        nCount_ = length();
    return substr(length() - nCount_, nCount_);
}
                
d4_inline int c4_String::Compare(const char* str_) const
{
    return compare(str_);
}

d4_inline bool c4_String::operator< (const c4_String& str_) const
{
    return compare(str_) < 0;
}

d4_inline int c4_String::Find(char ch_) const
{
    return find(ch_);
}

d4_inline int c4_String::ReverseFind(char ch_) const
{
    return rfind(ch_);
}

d4_inline int c4_String::FindOneOf(const char* set_) const
{
    return find_first_of(set_);
}

d4_inline int c4_String::Find(const char* sub_) const
{
    return find(sub_);
}

d4_inline c4_String c4_String::SpanIncluding(const char* set_) const
{
    return substr(0, find_first_not_of(set_));
}

d4_inline c4_String c4_String::SpanExcluding(const char* set_) const
{
    return substr(0, find_first_of(set_));
}

d4_inline bool operator== (const c4_String& a_, const c4_String& b_)
{
    return (d4_std::string) a_ == (d4_std::string) b_;
}

d4_inline bool operator!= (const c4_String& a_, const c4_String& b_)
{
    return (d4_std::string) a_ != (d4_std::string) b_;
}
    
d4_inline bool operator== (const c4_String& a_, const char* b_)
{
    return (d4_std::string) a_ == (d4_std::string) b_;
}

d4_inline bool operator== (const char* a_, const c4_String& b_)
{
    return (d4_std::string) a_ == (d4_std::string) b_;
}

d4_inline bool operator!= (const c4_String& a_, const char* b_)
{
    return (d4_std::string) a_ != (d4_std::string) b_;
}

d4_inline bool operator!= (const char* a_, const c4_String& b_)
{
    return (d4_std::string) a_ != (d4_std::string) b_;
}

#else                           // Universal replacement classes

d4_inline c4_String::c4_String ()
{
    Init(0, 0);
}

d4_inline c4_String::c4_String (const unsigned char* ptr)
{
    Init(ptr + 1, ptr ? *ptr : 0);
}

d4_inline c4_String::c4_String (const void* ptr, int len)
{
    Init(ptr, len);
}

d4_inline const c4_String& c4_String::operator+= (const c4_String& s)
{
    return *this = *this + s;
}

d4_inline const c4_String& c4_String::operator+= (const char* s)
{
    return *this += (c4_String) s;
}

d4_inline const char* c4_String::Data() const
{
    return (const char*) (_value + 2);
}

d4_inline c4_String::operator const char* () const
{
    return Data();
}

d4_inline c4_String::operator const unsigned char* () const
{
    return (unsigned char*) Data() - 1;
}

d4_inline char c4_String::operator[] (int i) const
{
    return Data()[i];
}
    
d4_inline int c4_String::GetLength() const
{
    return _value[1] != 255 ? _value[1] : FullLength();
}

d4_inline bool c4_String::IsEmpty() const
{
    return GetLength() == 0;
}

d4_inline void c4_String::Empty()
{
    *this = "";
}

d4_inline bool c4_String::operator< (const c4_String& a) const
{
    return Compare(a) < 0;
}

d4_inline c4_String operator+ (const char* a, const c4_String& b)
{
    return (c4_String) a + b;
}

d4_inline c4_String operator+ (const c4_String& a, const char* b)
{
    return a + (c4_String) b;
}

d4_inline bool operator== (const char* a, const c4_String& b)
{
    return b.Compare(a) == 0;
}

d4_inline bool operator== (const c4_String& a, const char* b)
{
    return a.Compare(b) == 0;
}

d4_inline bool operator!= (const c4_String& a, const c4_String& b)
{
    return !(a == b);
}

d4_inline bool operator!= (const char* a, const c4_String& b)
{
    return b.Compare(a) != 0;
}

d4_inline bool operator!= (const c4_String& a, const char* b)
{
    return a.Compare(b) != 0;
}

#endif // q4_UNIV

/////////////////////////////////////////////////////////////////////////////
