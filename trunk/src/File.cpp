#include "File.hpp"

// Include CoreFoundation here to prevent namespace collisions
#ifdef MACOSX
#include <CoreFoundation/CoreFoundation.h>
#endif


/*
 * Creates a new File object. Opens the file pointed to by path.
 *	path -> Path of file to open relative to DATADIR.
 *	readonly -> If true, do not open file for writing.
 * Throw std::runtime_error if file cannot be opened.
 */
File::File(const char *path, bool readonly)
   : stream(NULL), path(path)
{
   const char *mode = (readonly ? "rb" : "w+b");

#ifdef USE_FOPEN_S
   if (fopen_s(&stream, path, mode) != 0)
#else
      if ((stream = fopen(path, mode)) == NULL)
#endif
         throw std::runtime_error(string("Failed to open ") + path);
}


/*
 * Closes the file.
 */
File::~File()
{
   if (stream != NULL)
      fclose(stream);
}

/*
 * Reads a number of bytes from the file.
 *	buf -> Buffer to store data.
 *	bytes -> Number of bytes to read.
 * Throws an exception if all the requested bytes could not be read.
 */
void File::Read(void *buf, size_t bytes)
{
   size_t n = fread(buf, 1, bytes, stream);
   if (n != bytes)
      throw std::runtime_error("Failed to read from file: " + path);
}
 

/*
 * Writes a number of bytes to the file.
 *	buf -> Buffer containing data.
 *	bytes -> Number of bytes to writes.
 * Throws an exception if all the requested bytes could not be written.
 */
void File::Write(const void *buf, size_t bytes)
{
   size_t n = fwrite(buf, 1, bytes, stream);
   if (n != bytes)
      throw std::runtime_error("Failed to write to file: " + path);
}


/*
 * Sets the absolute read offset in the file.
 */
void File::Seek(size_t offset)
{
   fseek(stream, static_cast<long>(offset), SEEK_SET);
}


/*
 * Returns true if specified path points to a valid file.
 *	path -> File to check.
 */
bool File::Exists(const char *path)
{
#ifdef WIN32
   OFSTRUCT of;
   HFILE hFile;

   hFile = OpenFile(path, &of, OF_EXIST);
   return hFile != HFILE_ERROR;
#else
   return true;
#endif
}

/*
 * Locate a resource file in a platform independant manner.
 *  base -> Base of file name.
 *  ext -> Extension of file name.
 * Note that the returned pointer is only valid until the next call
 * to this function.
 */
const char *File::LocateResource(const char *base, const char *ext)
{
   static char path[MAX_RES_PATH];
	
#ifdef MACOSX
   CFURLRef resURL;
   CFBundleRef mainBundle;
   CFStringRef cfBase, cfExt, cfPath;
    
   cfBase = CFStringCreateWithCString(NULL, base, kCFStringEncodingASCII);
   cfExt = CFStringCreateWithCString(NULL, ext, kCFStringEncodingASCII);
    
   mainBundle = CFBundleGetMainBundle();
    
   resURL = CFBundleCopyResourceURL(mainBundle, cfBase, cfExt, NULL);
    
   if (resURL == NULL)
      throw runtime_error("Failed to locate " + string(base) + "." + string(ext));
	
   cfPath = CFURLCopyPath(resURL);
    
   CFStringGetCString(cfPath, path, MAX_RES_PATH, kCFStringEncodingASCII);
#else
   snprintf(path, MAX_RES_PATH, "%s/%s.%s", DATADIR, base, ext);
#endif

   return path;
}


