#include "hsv_to_rgb.h"
#include "stdio.h"

void print(uint8_t hsv[3], uint8_t rgb[3]) {
   printf("hsv : %x, %x, %x, rgb: %x %x %x\n", hsv[0], hsv[1], hsv[2], rgb[0], rgb[1], rgb[2]);
}

int main() {
   uint8_t hsv[3];
   uint8_t rgb[3];

   hsv[0] = 0;
   hsv[1] = 0;
   hsv[2] = 0;

   hsv_to_rgb(rgb, hsv);

   print(hsv, rgb);

   for (uint8_t i = 0; i < 255; i++) {
      hsv[0] = i;
      for (uint8_t j = 0; j < 255; j++) {
         hsv[1] = j;
         hsv_to_rgb(rgb, hsv);
         if (rgb[0] != 0 || 
               rgb[1] != 0 ||
               rgb[2] != 0) {
            printf(" crap %d %d\n", i, j);
            print(hsv, rgb);
         }
      }
   }
   printf("passed v being zero\n");

   //red
   printf("red\n");
   hsv[0] = 0;
   hsv[1] = 255;
   hsv[2] = 255;
   hsv_to_rgb(rgb, hsv);
   print(hsv, rgb);

   //cyan
   printf("cyan\n");
   hsv[0] = 127;
   hsv[1] = 127;
   hsv[2] = 255;
   hsv_to_rgb(rgb, hsv);
   print(hsv, rgb);

   //white
   printf("white\n");
   hsv[0] = 127;
   hsv[1] = 0;
   hsv[2] = 255;
   hsv_to_rgb(rgb, hsv);
   print(hsv, rgb);


   printf("round the world\n");
   hsv[1] = 255;
   hsv[2] = 255;

#if 0
   for(uint8_t i = 0; i < 255; i++) {
      hsv[0] = i;
      hsv_to_rgb(rgb, hsv);
      print(hsv, rgb);
   }
#endif

   return 0;
}
