#include <windows.h>
#include "idcard_gdiplus_util.h"
//////////////////////////////////////////////////////////////////////////
Bitmap* Utility::CreateBitmapFromMemory(const void *buf, size_t size)
{  
	IStream *stream = NULL;  
	HGLOBAL global = GlobalAlloc( GMEM_MOVEABLE, size );  
	if( NULL == global )  
		return NULL;  
	/* copy the buf content to the HGLOBAL */  
	if( !Memory2Global( buf, size, global ) )  
	{  
		GlobalFree( global );  
		stream->Revert();  

		return NULL;  
	}  
	/* get the IStream from the global object */  
	if( CreateStreamOnHGlobal( global, true, &stream ) != S_OK )  
	{  
		GlobalFree( global );  
		stream->Revert();  
		return NULL;  
	}  
	/* create the image from the stream */  
	Bitmap *image = Bitmap::FromStream( stream, FALSE );  
	//stream->Release();  
	//stream->Revert();  
	stream = NULL;  
	/* I suppose when the reference count for stream is 0, it will  
	GlobalFree automatically. The Image maintain the object also.*/   
	return image;  
}

bool Utility::Memory2Global(const void* buf, size_t size, HGLOBAL global)
{
	void* dest = GlobalLock(global);
	if (NULL == dest)
		return false;

	memcpy(dest, buf, size);
	GlobalUnlock(global);

	return true;
}
