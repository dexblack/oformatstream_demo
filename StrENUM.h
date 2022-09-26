//@doc
//@module StrENUM.h |
// Macro definitions which enable the declaration and definition of a function
// which translates an enumeration / bit flag value into a string.
//
// It is assumed that the values of the enumeration are named with the enumeration
// name as a prefix for each value.<nl>
// See DBTYPE in "oledb.h" for an example of a mixed strategy which requires both
// <f STR_ENUM_Mask> & <f STR_ENUM_FLAG> within the function definition.<nl>
// It is also assumed that the base type is comparable/compatible with DWORD.
//
//@end
//
//	Author:	dex
//	Date:	17/2/1999
//	Revision history:
//
#include <string.h>
#include <tchar.h>

//@func Macro | DECLARE_STR_ENUM_FUNC |
// Declare prototype for the conversion function.<nl>
// It is assumed that the type has a matching <p etype>##ENUM with members named etype_?
// 
//@parm TypeName | etype | Type to be converted to a string
//
#define DECLARE_STR_ENUM_FUNC(etype) LPCTSTR Str##etype(etype e)

//@func Macro | BEGIN_STR_ENUM_FUNC_MASK |
// Start the function definition with this macro if the input value <p etype>
// has any values which are NOT bit flags.
//
//@parm TypeName | etype | Type to be converted to a string
//@parm Hexadecimal | mask | Flag to use a mask for the non-bit-flag part of the value
//
#define BEGIN_STR_ENUM_FUNC_MASK(etype, mask) \
	BEGIN_STR_ENUM_FUNC_MASK_PREFIXLEN(etype, mask, _tcslen(_T(#etype)))

#define BEGIN_STR_ENUM_FUNC_MASK_NOPREFIX(etype, mask) \
	BEGIN_STR_ENUM_FUNC_MASK_PREFIXLEN(etype, mask, -1)

#define BEGIN_STR_ENUM_FUNC_MASK_PREFIXLEN(etype, mask, prefixlen) \
LPCTSTR Str##etype(etype e) { \
	const int maxEnumStrLen = 1000; \
	const DWORD m = mask; \
	static TCHAR str##etype[maxEnumStrLen+24]; \
	short eNameLen = prefixlen + 1; \
	TCHAR *s = str##etype, *s0 = s;

//@func Macro | BEGIN_STR_ENUM_FUNC |
// Start the function definition with this macro if the input value <p etype>
// has values which are always bit flags or combinations thereof.
//
//@parm TypeName | etype | Type to be converted to a string
//
#define BEGIN_STR_ENUM_FUNC(etype) BEGIN_STR_ENUM_FUNC_MASK(etype, 0xFFFFFFFF)
#define BEGIN_STR_ENUM_FUNC_NOPREFIX(etype) BEGIN_STR_ENUM_FUNC_MASK_NOPREFIX(etype, 0xFFFFFFFF)

//@func Macro | ENUMIF | Used internally for the two types of test required.<nl>
// See STR_ENUM_Mask and STR_ENUM_FLAG
//@parm EnumValue | n | The specific enumeration value to check for and add name of to output string
//@parm Hexadecimal | m | Mask value to use on <p n>
#define ENUMIF(n,m) \
	if ( (int)(s-s0) < maxEnumStrLen && \
		( ( (n) == 0 && (e) == (n) ) || ( (n) != 0 && ((e) & (m)) == (n) ) ) ) \
	{ \
		_tcscpy(s, _T(#n##" ") + eNameLen); \
		s += _tcslen(#n) + 1 - eNameLen; \
	}

//@func Macro | STR_ENUM_Mask |
// If the value 'n' occurs within a maskable range 'm'
// use this macro to set a specific mask range
//@parm EnumValue | n | The specific enumeration value to check for and add name of to output string
//@parm Hexadecimal | m | Mask value to use on <p n>
#define STR_ENUM_Mask(n) ENUMIF(n,m)

//@func Macro | STR_ENUM_FLAG |
// Use this macro if the enumeration value 'n' is a bit flag only
//@parm EnumValue | n | The specific enumeration value to check for and add name of to output string
#define STR_ENUM_FLAG(n) ENUMIF(n,n)

//@func Macro | END_STR_ENUM_FUNC | Terminates the function definition
#define END_STR_ENUM_FUNC \
	return s0; \
}
//------------------------------
// End of the macro definitions
//------------------------------

// If you wish to use an enumeration defined somewhere other than oledb.h
// include the requisite file prior to declaring the Str?() function for it.

#include <oledb.h>

DECLARE_STR_ENUM_FUNC(DBBINDSTATUS);	// LPCTSTR StrDBBINDSTATUS	(DBBINDSTATUS)
DECLARE_STR_ENUM_FUNC(DBTYPE);			// LPCTSTR StrDBTYPE		(DBTYPE)
DECLARE_STR_ENUM_FUNC(DBCOLUMNFLAGS);	// LPCTSTR StrDBCOLUMNFLAGS	(DBCOLUMNFLAGS)
DECLARE_STR_ENUM_FUNC(DBROWSTATUS);		// LPCTSTR StrDBROWSTATUS	(DBROWSTATUS)
DECLARE_STR_ENUM_FUNC(DBSTATUS);		// LPCTSTR StrDBSTATUS		(DBSTATUS)