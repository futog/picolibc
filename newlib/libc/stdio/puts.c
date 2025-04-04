/*
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * and/or other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
FUNCTION
<<puts>>---write a character string

INDEX
	puts
INDEX
	_puts_r

SYNOPSIS
	#include <stdio.h>
	int puts(const char *<[s]>);

	int puts( const char *<[s]>);

DESCRIPTION
<<puts>> writes the string at <[s]> (followed by a newline, instead of
the trailing null) to the standard output stream.

The alternate function <<_puts_r>> is a reentrant version.  The extra
argument <[reent]> is a pointer to a reentrancy structure.

RETURNS
If successful, the result is a nonnegative integer; otherwise, the
result is <<EOF>>.

PORTABILITY
ANSI C requires <<puts>>, but does not specify that the result on
success must be <<0>>; any non-negative value is permitted.

Supporting OS subroutines required: <<close>>, <<fstat>>, <<isatty>>,
<<lseek>>, <<read>>, <<sbrk>>, <<write>>.
*/

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "%W% (Berkeley) %G%";
#endif /* LIBC_SCCS and not lint */

#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include "fvwrite.h"
#include "local.h"

/*
 * Write the given string to stdout, appending a newline.
 */

int
puts (
       const char * s)
{
#ifdef __FVWRITE_IN_STREAMIO
  int result;
  size_t c = strlen (s);
  struct __suio uio;
  struct __siov iov[2];
  FILE *fp;

  iov[0].iov_base = s;
  iov[0].iov_len = c;
  iov[1].iov_base = "\n";
  iov[1].iov_len = 1;
  uio.uio_resid = c + 1;
  uio.uio_iov = &iov[0];
  uio.uio_iovcnt = 2;

  fp = stdout;
  CHECK_INIT();
  _newlib_flockfile_start (fp);
  if (ORIENT (fp, -1) != -1)
    result = EOF;
  else
    result = (_sfvwrite (fp, &uio) ? EOF : '\n');
  _newlib_flockfile_end (fp);
  return result;
#else
  int result = EOF;
  const char *p = s;
  FILE *fp;

  fp = stdout;
  CHECK_INIT();
  _newlib_flockfile_start (fp);
  /* Make sure we can write.  */
  if (cantwrite (ptr, fp))
    goto err;

  while (*p)
    {
      if (_sputc ( *p++, fp) == EOF)
	goto err;
    }
  if (_sputc ( '\n', fp) == EOF)
    goto err;

  result = '\n';

err:
  _newlib_flockfile_end (fp);
  return result;
#endif
}
