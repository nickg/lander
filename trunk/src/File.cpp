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
   : m_stream(NULL), m_path(path)
{
   const char *mode = (readonly ? "rb" : "w+b");

   //m_path = DATADIR;
   //m_path += path;

#ifdef USE_FOPEN_S
   if (fopen_s(&m_stream, m_path.c_str(), mode) != 0)
#else
      if ((m_stream = fopen(m_path.c_str(), mode)) == NULL)
#endif
         throw std::runtime_error("Failed to open " + m_path);
}


/*
 * Closes the file.
 */
File::~File()
{
   if (m_stream != NULL)
      fclose(m_stream);
}

/*
 * Reads a number of bytes from the file.
 *	buf -> Buffer to store data.
 *	bytes -> Number of bytes to read.
 * Throws an exception if all the requested bytes could not be read.
 */
void File::Read(void *buf, size_t bytes)
{
   size_t n = fread(buf, 1, bytes, m_stream);
   if (n != bytes)
      throw std::runtime_error("Failed to read from file: " + m_path);
}
 

/*
 * Writes a number of bytes to the file.
 *	buf -> Buffer containing data.
 *	bytes -> Number of bytes to writes.
 * Throws an exception if all the requested bytes could not be written.
 */
void File::Write(const void *buf, size_t bytes)
{
   size_t n = fwrite(buf, 1, bytes, m_stream);
   if (n != bytes)
      throw std::runtime_error("Failed to write to file: " + m_path);
}


/*
 * Sets the absolute read offset in the file.
 */
void File::Seek(size_t offset)
{
   fseek(m_stream, static_cast<long>(offset), SEEK_SET);
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


