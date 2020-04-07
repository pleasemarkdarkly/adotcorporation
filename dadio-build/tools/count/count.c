/* count.c: count the number of bytes in a file. echo the result
 * Dan Conti danc@iobjects.com 02/08/01
 */

#include <stdio.h>

int main( int argc, char** argv ) {
  FILE* fp;
  int result = 0;

  argc--; argv++;

  if( argc &&
      (fp = fopen( *argv, "r" )) ) {

    fseek( fp, 0, SEEK_END );
    result = ftell( fp );
    fclose( fp );
  }

  printf("%d\n", result );
  return 0;
}