#include <stdlib.h>
#include <err.h>
#include <sorting.h>
#include <string.h>

static inline const char* validate_args(u8 sd, u8 ss, u8 is, u32 lt, u32 ht) {
  if (sd != 'u' && sd != 'd' && sd != 'l' && sd != 'r') return "wrong sort direction";
  if (ss != 'h' && ss != 'l' && ss != 's') return "wrong sort style";
  if (is != 'n' && is != 't' && is != 'r') return "wrong interval style";
  if (lt > ht) return "invalid threshold";
  return NULL;
}

/*
  1. ARGV[1] - file input
  2. -o - output name
  3. -d - direction up/down/left/right
  4. -s - sorting style hue/light/saturation
  5. -i - interval style none/threshold
*/

int main(int argc, char *argv[])
{
  if (argc == 1)
    err_terminate("Usage: ./a.out <filename> <flags>:"
                  "-o <output file name>"
                  "-d <up/down/left/right> - direction"
                  "-s <hue/light> - sorting style"
                  "-i <none/threshold> <low threshold> <high_threshold> - interval style");

  const char* input_name = argv[1];
  char output_name[256] = "output.bmp";

  FILE* input_file = fopen(input_name, "rb");
  if (!input_file) err_terminate("File not found");
  
  u8 sort_direction  = 'l';
  u8 sort_style      = 'h';
  u8 interval_style  = 'n';
  u32 low_threshold  =   0;
  u32 high_threshold = 600;


  /*arg parsing*/
  for(i32 i = 1; i < argc; ++i)
  {
    if (!strcmp(argv[i], "-o") && (i + 1 < argc)) {
      strcpy(output_name, argv[i + 1]);
      i++; 
    } else if (!strcmp(argv[i], "-d") && (i + 1 < argc)) {
      sort_direction = argv[i + 1][0];
      i++; 
    } else if (!strcmp(argv[i], "-s") && (i + 1 < argc)) {
      sort_style = argv[i + 1][0];
      i++; 
    } else if (!strcmp(argv[i], "-i") && (i + 3 < argc)) {
      interval_style = argv[i + 1][0]; 
      low_threshold = atoi(argv[i + 2]); 
      high_threshold = atoi(argv[i + 3]); 
      i += 3;
    }     
  }
  
  const char* err = "";
  if((err = validate_args(
    sort_direction,
                            sort_style,
      interval_style,
                          low_threshold, 
    high_threshold
  ))) err_terminate(err);
  
  /*sorting*/
  BMP img; 
  if((err = open_bmp(&img, input_file)))
    err_terminate(err);

  pixel_sort(&img, sort_direction, sort_style, low_threshold, high_threshold);

  if((err = save_bmp(&img, output_name)))
    err_terminate(err);
  
  fclose(input_file);
  return 0;
}
