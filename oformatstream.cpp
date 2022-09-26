#include "stdafx.h"

#pragma warning ( disable : 4786 )

#include "oformatstream.hpp"

template <typename _E>
void __cdecl format_manip(std::ios_base& io, basic_formatter<_E>& f)
{
	basic_formatterfield<_E>& ff = f.next();
	io.width(ff.width);
	io.precision(ff.precision);
	io.flags(ff.flags);
}

void __cdecl formatspec_manip(std::ios_base& io, const format_specification& fs)
{
	io.width(fs.width);
	io.precision(fs.precision);
	io.flags(*const_cast<format_flags*>(&fs.flags));
}

std::_Smanip<formatter&> setformat(formatter& f)
{
	return std::_Smanip<formatter&>(&format_manip<char>, f);
}

std::_Smanip<wformatter&> setformat(wformatter& f)
{
	return std::_Smanip<wformatter&>(&format_manip<wchar_t>, f);
}

std::_Smanip<const format_specification&> setformat(const format_specification& fs)
{
	return std::_Smanip<const format_specification&>(&formatspec_manip, fs);
}

template <typename _E, typename _Tr>
void __cdecl reformat_manip(basic_oformatstream<_E,_Tr>& ofs,
							const basic_formatter<_E,_Tr>& f)
{
	ofs.formatter(f);
}

_FSmanip<char, const formatter&, std::char_traits<char> >
reformat(const formatter& f)
{
	return _FSmanip<char, const formatter&, std::char_traits<char> >
		(&reformat_manip<char,std::char_traits<char> >, f);
}

_FSmanip<wchar_t, const wformatter&, std::char_traits<wchar_t> >
reformat(const wformatter& f)
{
	return _FSmanip<wchar_t, const wformatter&, std::char_traits<wchar_t> >
		(&reformat_manip<wchar_t,std::char_traits<wchar_t> >, f);
}

//////////////////////////////////////////////////////////////////////
#if 0
#include <iostream>
#include <float.h>
void XIOSBASE_BUG()
{
	std::wcout << std::setiosflags(std::ios_base::fixed/* | std::ios_base::showpoint*/)
		<< DBL_MAX << std::endl;
	// this will cause an unhandled exception  (CRASH & BURN)
	// in XIOSBASE
	//	virtual _OI do_put(_OI _F, ios_base& _X, _E _Fill,
	//		double _V) const
	// the sprintf call overruns the _Buf array
	//
	// I tried fixing this by using snprintf but it returns -1
	// which translates to an unsigned long in the _Putc call later on.
}

#endif
//////////////////////////////////////////////////////////////////////