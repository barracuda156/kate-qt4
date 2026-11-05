/* qt4compat: kjob.h - there are no KJob-based asynchronous jobs in this
 * build (KIO is replaced by synchronous local file access, see
 * kio/netaccess.h); this header only lets sources that include it compile. */
#ifndef KDECOMPAT_KJOB_H
#define KDECOMPAT_KJOB_H

class KJob;

#endif
