#include "stdafx.h"

#pragma warning ( disable : 4786 )

#include <limits.h>

//!!!!!!!!!!!!!!!!!!
//#define USE_STDIO		// COMMENT THIS LINE OUT TO USE oformatstream
//!!!!!!!!!!!!!!!!!!
#if defined(USE_STDIO)

#define OUTPUT_FILE	"TestFormatOutput_STDIO.txt"

#include <stdio.h>
typedef char CHAR_T;
#define _CHAR_T(x) x

#ifndef _INC_FLOAT
#include <float.h>
#endif

#ifndef _INC_STRING
#include <string.h>
#endif

#ifndef _INC_STDLIB
#include <stdlib.h>
#endif

#else

#define OUTPUT_FILE	"TestFormatOutput_STD.txt"

#include <fstream>
#include "oformatstream.hpp"
typedef wchar_t CHAR_T;
#define _CHAR_T(x) L##x

#endif

// These control which sections of the output are printed to the file
bool SECTION_CHAR  = true;
bool SECTION_MISC  = true;
bool SECTION_INT   = true;
bool SECTION_FLOAT = true;

void TestFormat()
{
	float pi((float)3.1415);
	double e(2.71828);
	//long double p(6.21804e-23);

	char const c = 'c';
	char const cs[] = "cs";
	char const ca[] = "ca";
	wchar_t const wc = L'w';
	wchar_t const wcs[] = L"wcs";
	wchar_t const wca[] = L"wca";

#	define MAX_INTVALS 4
#	define MAX_FLTVALS 4
	int				i [MAX_INTVALS] = {INT_MAX, INT_MIN, 0xBED, -1};
	unsigned int	ui[MAX_INTVALS] = {UINT_MAX, 0, 0x8000, -1u};
	long			l[MAX_INTVALS] = { LONG_MAX, LONG_MIN, (long)0xFEED1BAD, -1l };
	unsigned long	ul[MAX_INTVALS] = {ULONG_MAX, 0, 0xF00D, -1ul};
	float			f [MAX_FLTVALS] = {FLT_MIN, FLT_MAX, FLT_EPSILON, pi};
	double			d [MAX_FLTVALS] = {DBL_MIN, DBL_MAX, DBL_EPSILON, e};
//	long double		ld[MAX_FLTVALS] = {LDBL_MIN, LDBL_MAX, LDBL_EPSILON, p};

	CHAR_T const* heading[] = {
	/* 0*/	_CHAR_T("------------------------------------------------------------------\n"),
	/* 1*/	_CHAR_T("CHARACTER TYPES char & wchar_t\n"),
	/* 2*/	_CHAR_T("INTEGER TYPES int, unsigned int, long, unsigned long\n"),
	/* 3*/	_CHAR_T("INT          UINT         LONG         ULONG       \n"),
	/* 4*/	_CHAR_T("FLOATING POINT TYPES float, double\n"),
	/* 5*/	_CHAR_T("FLOAT\t"),
	/* 6*/	_CHAR_T("DOUBLE\t"),
	//*  */	_CHAR_T("\n"),
		0
	};
	CHAR_T const* Format[] = {
	/* 0*/	_CHAR_T("%s%% tab[\x9] crlf"),
	/* 1*/	_CHAR_T("%s%% tab[\x9] crlf\n"),
	/* 2*/	_CHAR_T("%s%4c %5s %5s\\n"),
	/* 3*/	_CHAR_T("%s%-4c %-5s %-5s\n"),
	/* 4*/	_CHAR_T("%s%12d|%12u|%12ld|%12lu\n"),
	/* 5*/	_CHAR_T("%s%+12d|%+12u|%+12ld|%+12lu\n"),
	/* 6*/	_CHAR_T("%s%+-12d|%+-12u|%+-12ld|%+-12lu\n"),
	/* 7*/	_CHAR_T("%s%12d|%12u|%12ld|%12lu\n"),
	/* 8*/	_CHAR_T("%s%012x|%012x|%012lx|%012lx\n"),
	/* 9*/	_CHAR_T("%s%12X|%12X|%12lX|%12lX\n"),
	/*10*/	_CHAR_T("%s%#12x|%#12x|%#12lx|%#12lx\n"),
	/*11*/	_CHAR_T("%s%#12X|%#12X|%#12lX|%#12lX\n"),
	/*12*/	_CHAR_T("%s%12e|%12e|%12e|%12e\n"),
	/*13*/	_CHAR_T("%s%12E|%12E|%12E|%12E\n"),
	/*14*/	_CHAR_T("%s%12f|%12f|%12f|%12f\n"),
	/*15*/	_CHAR_T("%s%12g|%12g|%12g|%12g\n"),
	/*16*/	_CHAR_T("%s%12G|%12G|%12G|%12G\n"),
	/*17*/	_CHAR_T("%s%9.2e|%9.2e|%9.2e|%9.2e\n"),
	/*18*/	_CHAR_T("%s%9.2E|%9.2E|%9.2E|%9.2E\n"),
	/*19*/	_CHAR_T("%s%9.2f|%9.2f|%9.2f|%9.2f\n"),
	/*20*/	_CHAR_T("%s%9.2g|%9.2g|%9.2g|%9.2g\n"),
	/*21*/	_CHAR_T("%s%9.2G|%9.2G|%9.2G|%9.2G\n"),
	//*  */	_CHAR_T("%s"),
	//*  */	_CHAR_T("%s"),
		0
	};
	int const firstintformat = 4, lastintformat = 11;
	int const firstfloatformat = lastintformat+1, lastfloatformat = 21;

	char const filename[] = OUTPUT_FILE;

#if defined(USE_STDIO)
	FILE *stream = fopen(filename, "w" );
	if (!stream) return;
#else
	// Set up some default values
	std::wofstream outputfile(filename);
	std::wostream* defostream = &outputfile;//&std::wcout;
	std::wostream* errorstream = defostream;//&std::wcerr;
	std::streamsize defwidth(1), defprecision(2);
	std::ios_base::fmtflags defflags(
		std::ios_base::dec |
		std::ios_base::left |
		std::ios_base::scientific
		);
	format_specification defformatspec(defwidth, defprecision, defflags);

	// create an array of formatter objects
	wformatter format[] = {
		wformatter(Format[0]),
		wformatter(Format[1], defformatspec),
		wformatter(std::wstring(Format[2])),
		wformatter(std::wstring(Format[3]), defformatspec),
	};
	typedef std::vector<wformatter> formatter_vect;
	typedef formatter_vect::iterator formatter_vectit;

	formatter_vect intformat, floatformat;
	int intf = firstintformat;
	while(intf <= lastintformat)
	{
		intformat.push_back(wformatter(Format[intf++]));
	}
	while (intf <= lastfloatformat)
	{
		floatformat.push_back(wformatter(Format[intf++]));
	}
	// create an array of oformatstream objects
	// just to show the different constructors
	// ofs[3] is left to construct by default
	woformatstream ofs[4] = {
		woformatstream(std::wstring(Format[0]), defostream),
		woformatstream(format[0], defostream),
		woformatstream(format[0]),
	};
	ofs[2].tie(errorstream);
	ofs[3].formatter(format[0]);
	ofs[3].tie(defostream);
#endif

#if defined(USE_STDIO)
#	define HEADING(n)	fprintf(stream,"%s", heading[n]);
#else
	int ofsn = 1;//sizeof(ofs)/sizeof(ofs[0]);
	int ofsi = 0;
#	define HEADING(n)	*defostream << setformat(defformatspec) << heading[n]
#endif

	if (SECTION_CHAR)
	{
		HEADING(1);// CHARACTER TYPES
		HEADING(0);

		if (SECTION_MISC)
		{
#if defined(USE_STDIO)
			fprintf(stream, Format[0], Format[0]);
			fprintf(stream, "\n");
			fprintf(stream, Format[1], Format[1]);
			HEADING(0);
#else
			for (ofsi = 0; ofsi < ofsn; ++ofsi)
			{
				ofs[ofsi] << Format[0] << setformat << endl;
				ofs[ofsi] << reformat(format[1]);
				ofs[ofsi] << Format[1] << setformat;
				HEADING(0);
			}
#endif
		}

#if defined(USE_STDIO)
		fprintf(stream, Format[3], Format[3], c, cs, ca);
		int nchars = strlen(Format[3])+1;
		char* wideformat = new char[nchars];
		wchar_t* wformatstr = new wchar_t[nchars];
		memcpy(wideformat, Format[3], nchars);
		mbstowcs(wformatstr,Format[3],nchars);
		strupr(wideformat);
		fprintf(stream, wideformat, wformatstr, wc, wcs, wca);
		delete[] wideformat; wideformat = NULL;
		delete[] wformatstr; wformatstr = NULL;
#else
		for (ofsi = 0; ofsi < ofsn; ++ofsi)
		{
			ofs[ofsi] << reformat(format[2 + (ofsi%2)]);
			ofs[ofsi] << Format[2 + (ofsi%2)] << c << cs << ca << setformat;
			ofs[ofsi] << wc << wcs << wca << setformat;
			HEADING(0);
		}
#endif
	}
	if (SECTION_INT)
	{
		HEADING(2);// INTEGER TYPES
		HEADING(3);
		HEADING(0);

#if defined(USE_STDIO)
		for (int intf = firstintformat; intf <= lastintformat; ++intf)
		{
			for (int valindex = 0; valindex < MAX_INTVALS; ++valindex)
			{
				fprintf(stream, Format[intf], Format[intf],
						i[valindex], ui[valindex], l[valindex], ul[valindex]);
			}
			HEADING(0);
		}
#else
		for (ofsi = 0; ofsi < ofsn; ++ofsi)
		{
			formatter_vectit it = intformat.begin(), end = intformat.end();
			int intf = firstintformat;
			while (it != end)
			{
				ofs[ofsi] << reformat(*it++);
				for (int valindex = 0; valindex < MAX_INTVALS; ++valindex)
				{
					ofs[ofsi] << Format[intf] << i[valindex] << ui[valindex]
							  << l[valindex] << ul[valindex] << setformat;
				}
				++intf;
				HEADING(0);
			}
		}
#endif
	}
	if (SECTION_FLOAT)
	{
#if defined(USE_STDIO)
		for (int floatf = firstfloatformat; floatf <= lastfloatformat; ++floatf)
		{
			HEADING(5);
			fprintf(stream, Format[floatf], Format[floatf], f[0], f[1], f[2], f[3]);
			HEADING(6);
			fprintf(stream, Format[floatf], Format[floatf], d[0], d[1], d[2], d[3]);
			HEADING(0);
		}
#else
		int valindex = 0;
		for (ofsi = 0; ofsi < ofsn; ++ofsi)
		{
			formatter_vectit it = floatformat.begin(), end = floatformat.end();
			int floatf = firstfloatformat;
			while (it != end)
			{
				ofs[ofsi] << reformat(*it++);
				HEADING(5);
				ofs[ofsi] << Format[floatf];
				for (valindex = 0; valindex < MAX_FLTVALS; ++valindex)
				{
					ofs[ofsi] << f[valindex];
				}
				ofs[ofsi] << setformat;
				HEADING(6);
				ofs[ofsi] << Format[floatf];
				for (valindex = 0; valindex < MAX_FLTVALS; ++valindex)
				{
					ofs[ofsi] << d[valindex];
				}
				ofs[ofsi] << setformat;
				++floatf;
				HEADING(0);

			}
		}
#endif
	}
#if defined(USE_STDIO)
	   fclose( stream );
#endif
}