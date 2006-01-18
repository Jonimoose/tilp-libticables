// Helpers.cpp : convenient functions
#include "stdafx.h"

#include <wchar.h>

//* BSTR2CSTR() ******* USAGE INTERNE ***************************************
//*
//* Descriptif : Methode outil pour la conversion BSTR -> chaine C
//*
//* Param�tres en entr�e : 
//*   bstr_sting : Chaine d'entr�e de type BSTR
//*
//* Param�tres en sortie : aucun
//*
//* Param�tre de retour : pointeur char* sur chaine C
//*
//***************************************************************************

char* BSTR2CSTR(BSTR bstr_string)
{	
	static char buffer[512];

	wcsrtombs(buffer,(const unsigned short **)(&bstr_string),sizeof(buffer),0);
	return buffer;
}

//* CSTR2BSTR() ******* USAGE INTERNE ***************************************
//*
//* Descriptif : Methode outil pour la conversion Chaine C -> BSTR 
//*
//* Param�tres en entr�e : 
//*   cstr_sting : Chaine d'entr�e de type C
//*
//* Param�tres en sortie : aucun
//*
//* Param�tre de retour : pointeur wchar_t* sur chaine de type OLE
//*
//***************************************************************************

wchar_t * CSTR2BSTR(const char* cstr_string)
{	
	static wchar_t buffer[512];

	mbsrtowcs(buffer,&cstr_string,sizeof(buffer),0);
	return buffer;
}