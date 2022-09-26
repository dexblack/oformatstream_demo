//
// oformatstream.hpp
//
//
// Comments: std::ostream formatting object
//
// Example usage:
//
// First a really simple situation:
//
// oformatstream ofs("[%s] [%8d] [%6.5f]\n", &std::cout);
// ofs << "example" << 1 << 3.141592 << setformat;
//
// Note setformat is used to make sure the last field is output.
// The last field ?  Yes the "]\n" text.
// You could also drop the '\n' and use the endl inserter instead.
// This would result in the following code.
//
// oformatstream ofs("[%s] [%8d] [%6.5f]", &std::cout);
// ofs << "example" << 1 << 3.141592 << setformat << endl;
//
//
// A slightly more complex example:
//
// // Set up a custom default format specification first
// format_specification fs(12 /*width*/,2/*precision*/,std::ios_base::dec);
// // Generate a formatter with this default format specification
// formatter format("[%s] [%d] [%f]", fs);
// // Construct the oformatstream with this custom formatter
// oformatstream ofs(format, &std::cout);
// ofs << "example" << 1 << 3.141592 << endl;
//
// For some really complex examples see TestFormat.cpp
//
//
// Change History:
// Date			Name				Description
// 11/6/2001	dex@computer.org	Created
// 
//
//
/*
%[flags] [width] [.precision] [{h | l | I64 | L}]type

flags:
-	Left align the result within the given field width.  Right align.

+	Prefix the output value with a sign (+/-) if the output value is a signed type.
	Sign appears only for negative signed values.

0	If width is prefixed with 0, zeros are added until the minimum width is reached.
	If 0 and – appear, the 0 is ignored. If 0 is specified with an integer formatter
	(i, u, x, X, o, d) the 0 is ignored.  No padding. 
' '	Prefix the output value with a blank if the output value is signed and positive;
	the blank is ignored if both the blank and + flags appear. No blank appears. 
#	When used with the o, x, or X formatter, the # flag prefixes any nonzero
	output value with 0, 0x, or 0X, respectively. No blank appears. 
	When used with the e, E, or f formatter, the # flag forces the output value
	to contain a decimal point in all cases.
	Decimal point appears only if digits follow it.
	When used with the g or G formatter, the # flag forces the output value
	to contain a decimal point in all cases and prevents the truncation
	of trailing zeros. Ignored when used with c, d, i, u, or s.
	Decimal point appears only if digits follow it. Trailing zeros are truncated. 

type:
c	int	or wint_t A single-byte character;
d	int	Signed decimal integer. 
i	int Signed decimal integer. 
o	int Unsigned octal integer. 
u	int Unsigned decimal integer. 
x	int Unsigned hexadecimal integer, using “abcdef.” 
X	int Unsigned hexadecimal integer, using “ABCDEF.” 
e	double Signed value having the form [ – ]d.dddd e [sign]ddd
	where d is a single decimal digit, dddd is one or more decimal digits,
	ddd is exactly three decimal digits, and sign is + or –. 
E	double Same as 'e' except capital 'E' introduces the exponent instead.
f	double Signed value having the form [ – ]dddd.dddd,
	where dddd is one or more decimal digits.
	The number of digits before the decimal point depends on the magnitude
	of the number, and the number of digits after the decimal point
	depends on the requested precision. 
g	double Signed value printed in f or e formatter, whichever is more compact for
	the given value and precision. The e formatter is used only when the exponent
	of the value is less than –4 or greater than or equal to the precision argument.
	Trailing zeros are truncated, and the decimal point appears
	only if one or more digits follow it. 
G	double Identical to the g formatter, except that E, rather than e,
	introduces the exponent (where appropriate). 
s	String  A character string. Characters are printed up to the first null
	or until the precision value is reached.
p	Pointer to void. Prints the address pointed to by the argument in the form
	xxxxxxxx where x are uppercase hexadecimal digits. 

NOT SUPPORTED
n	Pointer to integer.
	Number of characters successfully written so far to the stream or buffer;
	this value is stored in the integer whose address is given as the argument. 

// fmtflags (prefix these with std:: as usual
uppercase,	converts to uppercase in certain insertions. 
showbase,	insert a prefix that reveals the base of a generated integer field. 
showpoint,	insert a decimal point unconditionally in a floating-point field. 
showpos,	insert a plus sign in a non-negative generated numeric field. 
left,		pad to field width by inserting fill at end (left justification). 
right,		pad to field width by inserting fill at beginning (right justification). 
internal,	pad to field width by inserting fill at a point internal
			to a generated numeric field. 
dec,		to insert or extract integer values in decimal formatter. 
oct,		to insert or extract integer values in octal formatter. 
hex,		to insert or extract integer values in hexadecimal formatter. 
scientific,	to insert floating-point values in scientific formatter
			(with an exponent field). 
fixed,		to insert floating-point values in fixed-point formatter
			(with no exponent field). 

NOT SUPPORTED (yet)
boolalpha,	to insert or extract objects of type bool as names
			(such as true and false) rather than as numeric values. 

*/

#ifndef _format_
#define _format_

#if !defined(_OSTREAM_)
#include <ostream>
#endif
#if !defined(_IOMANIP_)
#include <iomanip>
#endif
#ifndef _INC_MATH
#include <math.h>
#endif
#ifndef _INC_FLOAT
#include <float.h>
#endif
#ifndef _VECTOR_
#include <vector>
#endif
#ifndef _STRING_
//#include <string>
#endif

// A convenience macro (saves having to write std::ios_base:: everywhere)
#define SIB(x)	std::ios_base::x

#if defined(_DEBUG)
#include "StrENUM.h"
typedef SIB(_Fmtflags) IOS_BASE_FLAGS;
DECLARE_STR_ENUM_FUNC( IOS_BASE_FLAGS );
#endif

//------------------------------------------------------
// format_flags
// Manages changes to a std::ios_base::fmtflags value.
// Enforces restrictions such as hex,dec and oct being mutually exclusive flags.
// Provides operators |=, &=, = and various constructors and conversion operators.
//------------------------------------------------------
struct format_flags
{
	format_flags()
		: _flags(SIB(_Fmtzero))
	{}

	format_flags(const SIB(fmtflags) &flags)
		: _flags(SIB(_Fmtzero))
	{
		*this = flags;
	}

private:
	inline void toggle(SIB(fmtflags) mask, SIB(fmtflags) f)
	{ _flags &= ~(mask ^ f); _flags |= f; }

	inline bool is(SIB(fmtflags) what, SIB(fmtflags) f)
	{ return ((what & f) != SIB(_Fmtzero)); }

	inline void aligment()	// set default aligment
	{	// no alignment specified
		if (!is(SIB(adjustfield), _flags))
		{	// default fields to be right aligned
			toggle(SIB(adjustfield), SIB(right));
		}
	}

public:
	format_flags& operator=(const SIB(fmtflags) &flags)
	{
		_flags = SIB(_Fmtzero);
		_flags |= flags;
		return *this;
	}

	format_flags& operator|=(const SIB(_Fmtflags)& flags)
	{
		if (is(SIB(basefield), flags) || is(SIB(floatfield), flags))
		{
			if (is(SIB(basefield), flags))
			{
				if (is(SIB(dec), flags))	// biased to decimal format
				{
					toggle(SIB(basefield), SIB(dec));
				}
				else if (is(SIB(oct), flags))
				{
					toggle(SIB(basefield), SIB(oct));
				}
				else if (is(SIB(hex), flags))
				{
					toggle(SIB(basefield), SIB(hex));
				}
				_flags &= ~SIB(showpoint);
			}
			if (is(SIB(floatfield), flags))
			{
				if (is(SIB(scientific), flags))	// biased to scientific format
				{
					toggle(SIB(floatfield), SIB(scientific));
				}
				else if (is(SIB(fixed), flags))
				{
					toggle(SIB(floatfield), SIB(fixed));
				}
			}
		}
		if (is(SIB(adjustfield), flags))
		{
			if (is(SIB(right), flags))	// biased to right alignment
			{
				toggle(SIB(adjustfield), SIB(right));
			}
			else if (is(SIB(left), flags))
			{
				toggle(SIB(adjustfield), SIB(left));
			}
			else if (is(SIB(internal), flags))
			{
				toggle(SIB(adjustfield), SIB(internal));
			}
		}		

		// switch on any other flags that have been supplied
		_flags |= flags & ~(SIB(basefield) | SIB(floatfield) | SIB(adjustfield));
		aligment();
		return *this;
	}

	format_flags& operator|=(const SIB(fmtflags) &flags)
	{
		*this |= static_cast<const SIB(_Fmtflags)>(flags);
		return *this;
	}

	format_flags& operator&=(const SIB(_Fmtflags)& flags)
	{
		_flags &= flags;
		aligment();
		return *this;
	}

	format_flags& operator&=(const SIB(fmtflags) &flags)
	{
		*this &= static_cast<const SIB(_Fmtflags)>(flags);
		return *this;
	}

	operator SIB(fmtflags)()
	{
		return _flags;
	}

	operator SIB(_Fmtflags)()
	{
		return static_cast<SIB(_Fmtflags)>(_flags);
	}
private:
	SIB(fmtflags) _flags;
};

//------------------------------------------------------
// format_specification
//
// A single field's format specification
// corresponds to a single field ie. "%7.5f"
//------------------------------------------------------
struct format_specification
{
	format_specification()
		: width(1), precision(6), flags(SIB(_Fmtzero))
	{}

	format_specification(const format_specification& fs)
	{
		*this = fs;
	}

	format_specification(std::streamsize w, std::streamsize p,
		SIB(fmtflags) f = SIB(_Fmtzero))
		: width(w), precision(p), flags(f)
	{}

	format_specification& operator= (const format_specification& fs)
	{
		if (this != &fs)
		{
			flags = fs.flags;
			width = fs.width;
			precision = fs.precision;
		}
		return *this;
	}
	void reset()
	{
		*this = format_specification();
	}

	format_flags flags;			// formatting flags to use for field
	std::streamsize width;		// minimum width of output field
	std::streamsize precision;	// maximum significant digits (or width if string)
};

//------------------------------------------------------
// format_characters
//
// Provides a central place for storing constants required by the parsing routines.
// Currently _E may be either char or wchar_t.
//------------------------------------------------------
template <typename _E>
struct format_characters: private std::ctype<_E>
{
	inline _E blank()   { return std::ctype<_E>::widen(' '); }
	inline _E minus()   { return std::ctype<_E>::widen('-'); }
	inline _E plus()    { return std::ctype<_E>::widen('+'); }
	inline _E zero()    { return std::ctype<_E>::widen('0'); }
	inline _E hash()    { return std::ctype<_E>::widen('#'); }
	inline _E percent() { return std::ctype<_E>::widen('%'); }
	inline _E dot()     { return std::ctype<_E>::widen('.'); }

	inline _E c()       { return std::ctype<_E>::widen('c'); }
	inline _E d()       { return std::ctype<_E>::widen('d'); }
	inline _E i()       { return std::ctype<_E>::widen('i'); }
	inline _E o()       { return std::ctype<_E>::widen('o'); }
	inline _E u()       { return std::ctype<_E>::widen('u'); }
	inline _E x()       { return std::ctype<_E>::widen('x'); }
	inline _E X()       { return std::ctype<_E>::widen('X'); }
	inline _E e()       { return std::ctype<_E>::widen('e'); }
	inline _E E()       { return std::ctype<_E>::widen('E'); }
	inline _E f()       { return std::ctype<_E>::widen('f'); }
	inline _E g()       { return std::ctype<_E>::widen('g'); }
	inline _E G()       { return std::ctype<_E>::widen('G'); }
	inline _E n()       { return std::ctype<_E>::widen('n'); }
	inline _E p()       { return std::ctype<_E>::widen('p'); }
	inline _E s()       { return std::ctype<_E>::widen('s'); }

	// These are valid but ignored since operator<<() is type aware
	inline _E h()       { return std::ctype<_E>::widen('h'); }
	inline _E l()       { return std::ctype<_E>::widen('l'); }
	inline _E L()       { return std::ctype<_E>::widen('L'); }

	inline bool isFormatType(_E ch)
	{
		return
			(
			ch == c() ||
			ch == d() ||
			ch == i() ||
			ch == o() ||
			ch == u() ||
			ch == x() ||
			ch == X() ||
			ch == e() ||
			ch == E() ||
			ch == f() ||
			ch == g() ||
			ch == G() ||
			ch == p() ||
			ch == s() ||
			ch == l() ||
			ch == h() ||
			ch == L()
			);
	}
};

//------------------------------------------------------
// TEMPLATE CLASS basic_formatterfield
// Holds the final results of parsing a single field's format specification.
// These being a prefix text string and format specification for the field.
//------------------------------------------------------
template <typename _E, typename _Tr = std::char_traits<_E> >
struct basic_formatterfield : public format_specification
{
	basic_formatterfield() {
		format_characters<_E> fc;
		fill = fc.blank();
	}

	explicit basic_formatterfield(const format_specification& fs)
		: format_specification(fs)
	{
		format_characters<_E> fc;
		fill = fc.blank();
	}

	operator format_specification ()
	{
		return *static_cast<format_specification*>(this);
	}

	std::basic_string<_E,_Tr> text;	// plain text to be printed
	_E fill;
	void clear()
	{
		text.erase();
		format_specification::reset();
	}
};

//------------------------------------------------------
// TEMPLATE CLASS FormatFieldVector
//------------------------------------------------------
template <typename _E, typename _Tr = std::char_traits<_E> >
class  FormatFieldVector : public std::vector< basic_formatterfield<_E,_Tr> >
{};

//------------------------------------------------------
// Parsing routines
//------------------------------------------------------

//------------------------------------------------------
// format_flag_from_char
// Converts the type character [cdefgisx] into appropriate ios_base flag values.
//------------------------------------------------------
template <typename _E>
SIB(fmtflags) format_flag_from_char(const _E ch)
{
	format_flags flags;
	format_characters<_E> fc;

	if (ch == fc.E() ||
		ch == fc.G() ||
		ch == fc.p() ||
		ch == fc.X())			flags |= SIB(uppercase);

	if (ch == fc.blank())		flags |= SIB(right);
	else if (ch == fc.hash())	flags |= SIB(showbase);
	else if (ch == fc.plus())	flags |= SIB(showpos);
	else if (ch == fc.minus())	flags |= SIB(left);
	else if (ch == fc.c())		flags |= SIB(right);
	else if (ch == fc.s())		flags |= SIB(right);
	else if (ch == fc.d() ||
			 ch == fc.i() ||
			 ch == fc.u())		flags |= SIB(dec);
	else if (ch == fc.o())		flags |= SIB(oct);
	else if (ch == fc.x() ||
			 ch == fc.p() ||
			 ch == fc.X())		flags |= SIB(hex);
	else if (ch == fc.e() ||
			 ch == fc.E())		flags |= SIB(dec) | SIB(scientific);
	else if (ch == fc.f())		flags |= SIB(dec) | SIB(fixed);
	else if (ch == fc.g() ||
			 ch == fc.G())		flags |= SIB(dec), flags &= ~SIB(floatfield);
	return flags;
};

//------------------------------------------------------
// parse_format_specification
// Called by parse_field<_E>() to process a single field's format specification.
// ie. everything after the percent (%) symbol.
//------------------------------------------------------
template <typename _E, typename _Iter>
bool parse_format_specification(
	_Iter& it,
	_Iter& end,
	format_specification& outfs,
	bool& widthset, bool& precset,
	_E& fillchar)
{
	bool ok(true), done(false);
	widthset = precset = false;
	format_specification fs(0,0);
	SIB(fmtflags) ftemp(SIB(_Fmtzero));
	format_characters<_E> fc;
	SIB(fmtflags) flags(SIB(_Fmtzero));
	enum { inFlags, inWidth, inPrecision, inType} status = inFlags;
	while (it != end && !done && ok)
	{
		switch (status)
		{
		case inFlags:
			if (fc.zero() == *it)
			{
				fillchar = *it;
				break;
			}
			else if (std::isdigit(*it))
			{
				status = inWidth;	// fall through to next case
			}
			else if (fc.isFormatType(*it))
			{
				status = inType;		// fall straight through inType case
			}
			else
			{
				if (fc.hash()  == *it ||
					fc.plus()  == *it ||
					fc.minus() == *it)
				{
					fs.flags |= format_flag_from_char(*it);
				}
				else if (fc.blank() == *it)
				{
					fillchar = *it;
				}
				else
				{
					status = inWidth;
				}
				break;
			}
		case inWidth:
			if (inWidth == status)
			{
				if (std::isdigit(*it))
				{
					fs.width *= 10;
					fs.width += (*it - fc.zero());
					widthset = true;
					break;
				}
				else if (fc.dot() == *it)
				{
					status = inPrecision;	// fall through to inPrecision case
					fs.flags |= SIB(showpoint);
					++it;
					if (it == end)
					{
						ok = false;
						done = true;
						break;
					}
				}
				else if (fc.isFormatType(*it))
				{
					status = inType;		// fall straight through inType case
				}
				else
				{
					ok = false;
				}
			}	// otherwise falling through
		case inPrecision:
			if (inPrecision == status)
			{
				if (std::isdigit(*it))
				{
					fs.precision *= 10;
					fs.precision += (*it - fc.zero());
					precset = true;
					break;
				}
				else if (fc.isFormatType(*it))
				{
					status = inType;		// fall straight through inType case
				}
				else
				{
					ok = false;
				}
			}	// otherwise falling through
		case inType:
			if (fc.blank() == *it)
			{
				done = true;
			}
			else
			{
				ftemp = format_flag_from_char(*it);
				if (SIB(_Fmtzero) != ftemp)
				{
					fs.flags |= ftemp;
				}
				else if (!fc.isFormatType(*it))
				{
					done = true;
				}
			}
			break;
		}
		if (!done && it != end) ++it;
	}
	if (ok)
	{
		if (!fs.width)
		{
			fs.width = 1;
		}
		if (!fs.precision &&
			((SIB(basefield) | SIB(floatfield))
			 & static_cast<SIB(fmtflags)>(fs.flags)))
		{
			fs.precision = 6;
		}
		outfs = fs;
	}
	return ok;
}

//------------------------------------------------------
// parse_field
// Called by parse_format<_E> to process a format field.
// ie. The prefix text followed by a format specification.
// Calls parse_format_specification<_E>()
//------------------------------------------------------
template<typename _E, typename _Iter>
bool parse_field(_Iter& it, _Iter& end,
				 basic_formatterfield<_E>& outff,
				 format_specification& default_fs)
{
	bool ok(true), done(false), widthset(false), precset(false);
	format_characters<_E> fc;
	enum { inText, inField } status = inText;
	outff.clear();
	while (it != end && !done && ok)
	{
		switch (status)
		{
		case inText:
			if (fc.percent() == *it)
			{
				status = inField;
			}
			else
			{
				outff.text += *it;
			}
			break;
		case inField:
			// Parse a complete format_specification
			if (fc.percent() == *it)
			{
				outff.text += *it;
				status = inText;
			}
			else
			{
				ok = parse_format_specification<_E>(it,end,outff,
					widthset,precset,outff.fill);
				done = true;
			}
			break;
		}
		if (!done && it != end) ++it;
	}
	if (!widthset)
	{
		outff.width = default_fs.width;
	}
	if (!precset)
	{
		outff.precision = default_fs.precision;
	}
	return ok;
}

//------------------------------------------------------
// parse_format
// Used by basic_formatter<_E> constructors to process a full format specification.
// Calls parse_field<_E>() to build a basic_formatterfield which
// it then stores in a FormatFieldVector.
//------------------------------------------------------
template <typename _E>
bool parse_format(
	std::basic_string<_E> fs,
	FormatFieldVector<_E>& ffv,
	format_specification& default_fs)
{
	bool ok(true);
	auto it = fs.begin(), end = fs.end();
	while (it != end && ok)
	{
		basic_formatterfield<_E> ff;
		ok = parse_field(it,end,ff,default_fs);
		if (ok)
		{
			ffv.push_back(ff);
		}
	}
	return ok;
}

//------------------------------------------------------
// TEMPLATE CLASS basic_formatter
// The format of each field is controlled by the given format string.
//------------------------------------------------------
template <typename _E, typename _Tr = std::char_traits<_E> >
class basic_formatter
{
public:
	basic_formatter()
		: _ok(true), _curff()
	{}

	basic_formatter(const format_specification fs)
		: _ok(true), _curff(0), _default_format(fs)
	{}
	
	basic_formatter(std::basic_string<_E,_Tr> fs)
		: _ok(true)
	{
		_ok = parse_format(fs, _ffv, _default_format);
		_curff = _ffv.begin();
	}

	basic_formatter(std::basic_string<_E,_Tr> s, const format_specification& fs)
		: _ok(true), _default_format(fs)
	{
		_ok = parse_format(s, _ffv, _default_format);
		_curff = _ffv.begin();
	}

	basic_formatter(const basic_formatter& f)
	{
		*this = f;
	}

	basic_formatter& operator=(const basic_formatter& f)
	{
		if (this != &f)
		{
			_ok = f._ok;	// you're _ok I'm _ok
			_ffv = f._ffv;	// copy formatter field vector
			_curff = _ffv.begin();	// restart at first formatter field on copy
		}
		return (*this);
	}

	void default_format_specification(const format_specification& f)
	{
		_default_format = f;
	}
	format_specification default_format_specification()
	{
		return _default_format;
	}

	bool isValid()
	{ return _ok; }

	FormatFieldVector<_E>::size_type FieldCount()
	{ return _ffv.size(); }

	basic_formatterfield<_E>& operator() ()
	{ return (_curff != _ffv.end() ? *_curff : _default_format); }

	// This is the heart of the whole operation.
	// Each time a field is output the custom setformat() function
	// calls this routine and shifts _curff to the next basic_formatterfield
	basic_formatterfield<_E>& next()
	{
		basic_formatterfield<_E>& ret = operator()();
		if (!_ffv.empty() && ++_curff == _ffv.end()) _curff = _ffv.begin();
		return ret;
	}

private:
	bool _ok;
	FormatFieldVector<_E,_Tr> _ffv;
	FormatFieldVector<_E,_Tr>::iterator _curff;
	basic_formatterfield<_E> _default_format;
};

typedef basic_formatter<char, std::char_traits<char> > formatter;
typedef basic_formatter<wchar_t, std::char_traits<wchar_t> > wformatter;


//------------------------------------------------------
// TEMPLATE CLASS basic_oformatstream
// Outputs values to the connected stream (does nothing if not).
// The internal basic_formatter object controls the layout of each
// output field and the order that they are expected.
// No exceptions are thrown if the supplied field type does not match
// the expected format. The output will probably just look awful.
//------------------------------------------------------
template <typename _E, typename _Tr = std::char_traits<_E> >
class basic_oformatstream
{
public:

	typedef basic_oformatstream<_E,_Tr> _Myt;
	typedef std::basic_ostream<_E,_Tr> _Myostream;
	typedef _Myostream::_Mysb _Mysb;
	typedef _Myostream::_Myios _Myios;

	basic_oformatstream()
		: _Ostream(NULL)
	{}

	explicit basic_oformatstream(const std::basic_string<_E,_Tr>& s, _Myostream *os = NULL)
		: _Format(basic_formatter<_E,_Tr>(s)), _Ostream(NULL)
	{ tie(os); }

	explicit basic_oformatstream(const basic_formatter<_E>& f, _Myostream *os = NULL)
		: _Format(f), _Ostream(NULL)
	{ tie(os); }

	virtual ~basic_oformatstream()
	{}

	void formatter(const basic_formatter<_E,_Tr>& f)
	{ _Format = f; }

	basic_formatter<_E>& formatter()
	{ return _Format; }

	void tie(_Myostream *os)
	{ _Ostream = os; }

	_Myostream* get_ostream()
	{ return _Ostream; }

	format_specification default_format_specification()
	{ return _Format.default_format_specification(); }

	// MANIPULATION OPERATIONS

	_Myt& operator<<(_Myt& (__cdecl *_F)(_Myt&))
	{ return ((*_F)(*this)); }

	_Myt& operator<<(_Myostream& (__cdecl *_F)(_Myostream&))
	{ if (_Ostream) ((*_F)(*_Ostream)); return (*this); }

	_Myt& operator<<(_Myios& (__cdecl *_F)(_Myios&))
	{ if (_Ostream) (*_F)(*(_Myios *)_Ostream); return (*this); }

	_Myt& operator<<(std::ios_base& (__cdecl *_F)(std::ios_base&))
	{ if (_Ostream) (*_F)(*(std::ios_base *)_Ostream); return (*this); }

	bool prefix()
	{
		bool ok(NULL != _Ostream);
		if (ok)
		{
			std::basic_string<_E,_Tr>& text = _Format().text;
			if (!text.empty())
				*_Ostream << setformat(_Format.default_format_specification())
					<< text.c_str();
			_Ostream->fill(_Format().fill);
			*_Ostream << setformat(_Format);
		}
		return ok;
	}
	void suffix()
	{
		if (NULL != _Ostream)
		{
			*_Ostream << setformat(_Format.default_format_specification());
		}
	}

	// INSERTER operators
	_Myt& operator<<(bool _X)
	{if (prefix())	{_Ostream->operator<<(_X); suffix(); }
		return (*this); }
	_Myt& operator<<(short _X)
	{if (prefix())	{_Ostream->operator<<(_X); suffix(); }
		return (*this); }
	_Myt& operator<<(unsigned short _X)
	{if (prefix())	{_Ostream->operator<<(_X); suffix(); }
		return (*this); }
	_Myt& operator<<(int _X)
	{if (prefix())	{_Ostream->operator<<(_X); suffix(); }
		return (*this); }
	_Myt& operator<<(unsigned int _X)
	{if (prefix())	{_Ostream->operator<<(_X); suffix(); }
		return (*this); }
	_Myt& operator<<(long _X)
	{if (prefix())	{_Ostream->operator<<(_X); suffix(); }
		return (*this); }
	_Myt& operator<<(unsigned long _X)
	{if (prefix())	{_Ostream->operator<<(_X); suffix(); }
		return (*this); }
	_Myt& operator<<(float _X)
		{*this<<((double)_X);
		return (*this); }
	// The complexity in this inserter is due to the need to emulate
	// %g format (general floating point). This code adjusts the precision
	// and fixed vs scientific format flags.
	_Myt& operator<<(double _X)
		{if (prefix()){SIB(fmtflags) _f = _Ostream->flags();
			if (!(_f & SIB(floatfield))){
				std::streamsize prec = _Ostream->precision();
				double mant = fabs(_X);
				if (mant < 1e-4 || (mant > 0.0 ? (log10(mant) >= prec) : true)){
					_Ostream->setf(SIB(scientific));
				}
				else {
					_Ostream->setf(SIB(fixed));
				}
				_Ostream->precision(prec>0?prec-1:prec);
				if (!(_Ostream->flags() & SIB(showpoint))){
					mant = _X * pow(-log10(mant),10);
					if ((mant - (double)(__int64)mant) > 10 * DBL_EPSILON)
					{
						_Ostream->setf(SIB(showpoint));
					}
				}
			}
			_Ostream->operator<<(_X); suffix();}
		return (*this); }
	_Myt& operator<<(long double _X)
		{if (prefix())	{_Ostream->operator<<(_X); suffix(); }
		return (*this); }
	_Myt& operator<<(const void *_X)
		{if (prefix())	{_Ostream->operator<<(_X); suffix(); }
		return (*this); }
	_Myt& operator<<(_Mysb *_Pb)
		{if (prefix())  _Ostream->operator<<(_Pb);
		return (*this); }

	_Myt& flush()
		{if (_Ostream) { _Ostream->flush(); }
		return (*this); }

	_Myt& put(_E _X)
		{if (_Ostream) { _Ostream->put(_X); }
		return (*this); }

	_E widen(char _C) const
		{return (_Ostream ? _Ostream->widen(_C): _E(0)); }

private:
	basic_formatter<_E> _Format;
	_Myostream *_Ostream;
};


	// INSERTERS
template<class _E, class _Tr> inline
basic_oformatstream<_E, _Tr>& __cdecl operator<<(
	basic_oformatstream<_E, _Tr>& _O, const _E *_X)
{
	if (_O.prefix()) {
		auto* os = _O.get_ostream();
		*os << _X;
		_O.suffix();
	}
	return (_O); 
}

template<class _E, class _Tr> inline
basic_oformatstream<_E, _Tr>& __cdecl operator<<(
	basic_oformatstream<_E, _Tr>& _O, _E _C)
{
	if (_O.prefix()) {
		auto os = _O.get_ostream();
		*os << _C;
		_O.suffix();
	}
	return (_O); 
}

template<class _E, class _Tr> inline
basic_oformatstream<_E, _Tr>& __cdecl operator<<(
	basic_oformatstream<_E, _Tr>& _O, const signed char *_X)
{
	return (_O << (const char *)_X);
}

template<class _E, class _Tr> inline
basic_oformatstream<_E, _Tr>& __cdecl operator<<(
	basic_oformatstream<_E, _Tr>& _O, const signed char _C)
{
	return (_O << (char)_C);
}

template<class _E, class _Tr> inline
basic_oformatstream<_E, _Tr>& __cdecl operator<<(
	basic_oformatstream<_E, _Tr>& _O, const unsigned char *_X)
{
	return (_O << (const char *)_X);
}

template<class _E, class _Tr> inline
basic_oformatstream<_E, _Tr>& __cdecl operator<<(
	basic_oformatstream<_E, _Tr>& _O, const unsigned char _C)
{return (_O << (char)_C); }

template<class _E, class _Tr> inline
basic_oformatstream<_E, _Tr>& __cdecl operator<<(
	basic_oformatstream<_E, _Tr>& _O, const signed short *_X)
{return (_O << (const wchar_t *)_X); }


	// IOSTREAM MANIPULATORS
std::_Smanip<const format_specification&> setformat(const format_specification&);
std::_Smanip<formatter&> setformat(formatter&);
std::_Smanip<wformatter&> setformat(wformatter&);

	// FORMATSREAM MANIPULATORS
template <typename _E, typename _Tm, typename _Tr = std::char_traits<_E> >
struct _FSmanip
{
	_FSmanip(void (__cdecl *_F)(basic_oformatstream<_E,_Tr>&, _Tm), _Tm _A)
		: _Pf(_F), _Manarg(_A) {}
	void (__cdecl *_Pf)(basic_oformatstream<_E,_Tr>&, _Tm);
	_Tm _Manarg;
};
template<class _E, class _Tr, class _Tm> inline
	basic_oformatstream<_E, _Tr>& __cdecl operator<<(
		basic_oformatstream<_E, _Tr>& _O, const _FSmanip<_E,_Tm,_Tr>& _M)
	{(*_M._Pf)(_O, _M._Manarg);
	return (_O); }

_FSmanip<char, const formatter&,  std::char_traits<char> > reformat(const formatter&);
_FSmanip<wchar_t, const wformatter&, std::char_traits<wchar_t> > reformat(const wformatter&);


template<class _E, class _Tr> inline
basic_oformatstream<_E, _Tr>&
__cdecl endl(basic_oformatstream<_E, _Tr>& _O)
{_O.put(_O.widen('\n'));_O.flush();return (_O);}
inline basic_oformatstream<char, std::char_traits<char> >&
__cdecl endl(basic_oformatstream<char, std::char_traits<char> >& _O)
{_O.put('\n');_O.flush();return (_O);}
inline basic_oformatstream<wchar_t, std::char_traits<wchar_t> >&
__cdecl endl(basic_oformatstream<wchar_t, std::char_traits<wchar_t> >& _O)
{_O.put('\n');_O.flush();return (_O);}

template<class _E, class _Tr> inline
basic_oformatstream<_E, _Tr>&
__cdecl ends(basic_oformatstream<_E, _Tr>& _O)
{_O.put(_E('\0'));return (_O);}
inline basic_oformatstream<char, std::char_traits<char> >&
__cdecl ends(basic_oformatstream<char, std::char_traits<char> >& _O)
{_O.put('\0');return (_O);}
inline basic_oformatstream<wchar_t, std::char_traits<wchar_t> >&
__cdecl ends(basic_oformatstream<wchar_t, std::char_traits<wchar_t> >& _O)
{_O.put('\0');return (_O);}

template<class _E, class _Tr> inline
basic_oformatstream<_E, _Tr>&
__cdecl flush(basic_oformatstream<_E, _Tr>& _O)
{_O.flush();return (_O);}
inline basic_oformatstream<char, std::char_traits<char> >&
__cdecl flush(basic_oformatstream<char, std::char_traits<char> >& _O)
{_O.flush();return (_O);}
inline basic_oformatstream<wchar_t, std::char_traits<wchar_t> >&
__cdecl flush(basic_oformatstream<wchar_t, std::char_traits<wchar_t> >& _O)
{_O.flush();return (_O); }

template<class _E, class _Tr> inline
basic_oformatstream<_E, _Tr>&
__cdecl setformat(basic_oformatstream<_E,_Tr>& _O)
{
	auto os = _O.get_ostream();
	if (os)
	{
		basic_formatter<_E>& f = _O.formatter();
		if (!f().text.empty())
			*os << setformat(f.default_format_specification()) << f().text.c_str();
		*os << setformat(f);
	}
	return (_O);
}
inline basic_oformatstream<char, std::char_traits<char> >&
__cdecl setformat(basic_oformatstream<char, std::char_traits<char> >& _O)
{
	auto os = _O.get_ostream();
	if (os)
	{
		basic_formatter<char>& f = _O.formatter();
		if (!f().text.empty())
			*os << setformat(f.default_format_specification())
				<< f().text.c_str();
		*os << setformat(f);
	}
	return (_O);
}
inline basic_oformatstream<wchar_t, std::char_traits<wchar_t> >&
__cdecl setformat(basic_oformatstream<wchar_t, std::char_traits<wchar_t> >& _O)
{
	basic_oformatstream<wchar_t,std::char_traits<wchar_t> >::_Myostream*
		os = _O.get_ostream();
	if (os)
	{
		basic_formatter<wchar_t>& f = _O.formatter();
		if (!f().text.empty())
			*os << setformat(f.default_format_specification())
				<< f().text.c_str();
		*os << setformat(f);
	}
	return (_O);
}


typedef basic_oformatstream<char, std::char_traits<char> > oformatstream;
typedef basic_oformatstream<wchar_t, std::char_traits<wchar_t> > woformatstream;

#undef SIB
#endif	// _format_
