/*******************************************************
*   MNGTranslator©
*
*   @author  YNOP (ynop@acm.org)
*   @version 1
*   @date    Jun 23 2001
*******************************************************/
#include <StorageKit.h>
#include <Bitmap.h>

#include "libmng.h"
#include "MNGTranslator.h"

/*******************************************************
*   
*******************************************************/
mng_ptr myalloc(mng_size_t size){
	return (mng_ptr)calloc(1, size);
}

/*******************************************************
*   
*******************************************************/
void myfree(mng_ptr p, mng_size_t size){
	free(p);
	return;
}

/*******************************************************
*   
*******************************************************/
mng_bool mymngopenstream(mng_handle mng){
   mngstuff	*mymng = NULL;
   mymng = (mngstuff*)mng_get_userdata(mng);
   return MNG_TRUE;
}

/*******************************************************
*   
*******************************************************/
mng_bool mymngclosestream(mng_handle mng){
   mngstuff	*mymng = NULL;
   mymng = (mngstuff*)mng_get_userdata(mng);
   return MNG_TRUE;
}

/*******************************************************
*   
*******************************************************/
mng_bool mymngreadstream(mng_handle mng, mng_ptr buffer,mng_uint32 size, mng_uint32 *bytesread){
   mngstuff *mymng = NULL;
   mymng = (mngstuff*)mng_get_userdata(mng);

   *bytesread = mymng->inSource->Read(buffer,size);
   
   return MNG_TRUE;
}

/*******************************************************
*   
*******************************************************/
mng_bool mymngprocessheader(mng_handle mng,mng_uint32 width, mng_uint32 height){
   mngstuff *mymng = NULL;
   mymng = (mngstuff*)mng_get_userdata(mng);
   
   mymng->type = mng_get_imagetype(mng);

   switch(mymng->type){
   case mng_it_unknown:
      //printf("Not one of ours\n");
      return MNG_FALSE;
      break;
   case mng_it_png:
      // we should add a user set var here that lets them 
      // decide if they want to use the PNG trans or us.
      if(!mymng->settings.transPNG){
         //printf("Leting PNG handle this\n");
         return MNG_FALSE;
      }else{
         //printf("MNG transing a PNG\n");
      }
      break;
   case mng_it_mng:
      //printf("Video file, will return first frame\n");
      break;
   case mng_it_jng:
      break;
   default:
      return MNG_FALSE;
      break;
   }   
   
   mng_set_canvasstyle(mng, MNG_CANVAS_BGRA8);
   
	BRect r(0,0,width-1,height-1);
   mymng->bmap = new BBitmap(r,B_RGBA32);
   
   return MNG_TRUE;
}

/*******************************************************
*   
*******************************************************/
mng_bool mymngrefresh(mng_handle mng, mng_uint32 x, mng_uint32 y,mng_uint32 w, mng_uint32 h){
   mngstuff	*mymng = NULL;
   mymng = (mngstuff*)mng_get_userdata(mng);
   return MNG_TRUE;
}

/*******************************************************
*   
*******************************************************/
mng_ptr mymnggetcanvasline(mng_handle mng, mng_uint32 line){
   mngstuff *mymng = NULL;
   mymng = (mngstuff*)mng_get_userdata(mng);
   
   int32 *bits = (int32*)mymng->bmap->Bits();
   bits = bits + (mymng->bmap->BytesPerRow()/4 * line);
	return bits;
}

/*******************************************************
*   
*******************************************************/
mng_bool mymngerror(mng_handle mng, mng_int32 code, mng_int8 severity,mng_chunkid chunktype, mng_uint32 chunkseq,	mng_int32 extra1, mng_int32 extra2, mng_pchar text){
   mngstuff *mymng = NULL;
	
   mymng = (mngstuff*)mng_get_userdata(mng);
   
   printf("Error... die die die (code %i, severity %i)\n",code,severity);
	return (0);
}

/*******************************************************
*   
*******************************************************/
mng_bool mymngsettimer(mng_handle mng, mng_uint32 msecs){
   mngstuff	*mymng = NULL;
   mymng = (mngstuff*)mng_get_userdata(mng);
   
   mymng->delay = msecs;
	return MNG_TRUE;
}

/*******************************************************
*   
*******************************************************/
mng_uint32 mymnggetticks(mng_handle mng){
   return 0; // that aint right.
}



