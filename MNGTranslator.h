#ifndef _MNG_TRANSLATOR_H
#define _MNG_TRANSLATOR_H

#include <TranslatorAddOn.h>
#include <TranslationKit.h>

#include "libmng.h"

/*******************************************************
*   The Public Name of the Translator and some niffty
*   info that we want the world to see.
*******************************************************/
#define MNG_TYPE 'mng '
#define MNG_MIME_IMAGE "image/x-jng"
#define MNG_MIME_VIDEO "video/x-mng"
#define MNG_NAME "MNG File format"

char translatorName[] = "MNGTranslator";
char translatorInfo[] = "libmng Version: "MNG_VERSION_TEXT;
int32 translatorVersion = MNG_VERSION_MAJOR * 100 +  MNG_VERSION_MINOR * 10 + MNG_VERSION_RELEASE;

const float MNG_INPUT_QUALITY = 0.5;
const float MNG_INPUT_CAPABILITY = 0.6;

/*******************************************************
*   A list of what formats we can support
*******************************************************/
translation_format inputFormats[] = {
   //{ B_TRANSLATOR_BITMAP, B_TRANSLATOR_BITMAP, 0.4, 0.8, "image/x-be-bitmap", "Be Bitmap Format (SGIHandler)" },
   { MNG_TYPE, B_TRANSLATOR_BITMAP, MNG_INPUT_QUALITY, MNG_INPUT_CAPABILITY, MNG_MIME_IMAGE, MNG_NAME },
   { MNG_TYPE, B_TRANSLATOR_BITMAP, MNG_INPUT_QUALITY, MNG_INPUT_CAPABILITY, MNG_MIME_VIDEO, MNG_NAME },
   { 0, 0, 0, 0, "\0", "\0" }
};
   
/*******************************************************
*  A list of formats that we can write too. 
*******************************************************/
translation_format outputFormats[] = {
	{	B_TRANSLATOR_BITMAP, B_TRANSLATOR_BITMAP, 0.4, 0.7, "image/x-be-bitmap", "Be Bitmap Format" },
	//{	MNG_TYPE, B_TRANSLATOR_BITMAP, 0.3, 0.8, MNG_MIME, MNG_NAME },
	{	0, 0, 0, 0, "\0", "\0" }
};	


struct MNG_settings{
   bool transPNG;
   bool touched;
};

// Little stuct we pass around so that we can
// keep track of what we are doing and make sure
// that we don't cross threads and get confused
typedef struct {
	BPositionIO *inSource;
	BBitmap *bmap;
	mng_uint32	delay;
	mng_imgtype type;
	MNG_settings settings;
} mngstuff;


#endif
