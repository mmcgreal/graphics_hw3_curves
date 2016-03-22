#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "parser.h"


/*======== void parse_file () ==========
Inputs:   char * filename 
          struct matrix * transform, 
          struct matrix * pm,
          screen s
Returns: 

Goes through the file named filename and performs all of the actions listed in that file.
The file follows the following format:
     Every command is a single character that takes up a line
     Any command that requires arguments must have those arguments in the second line.
     The commands are as follows:
         line: add a line to the edge matrix - 
	    takes 6 arguemnts (x0, y0, z0, x1, y1, z1)
	 circle: add a circle to the edge matrix - 
	    takes 3 arguments (cx, cy, r)
	 hermite: add a hermite curve to the edge matrix -
	    takes 8 arguments (x0, y0, x1, y1, x2, y2, x3, y3)
	 bezier: add a bezier curve to the edge matrix -
	    takes 8 arguments (x0, y0, x1, y1, x2, y2, x3, y3)
	 ident: set the transform matrix to the identity matrix - 
	 scale: create a scale matrix, 
	    then multiply the transform matrix by the scale matrix - 
	    takes 3 arguments (sx, sy, sz)
	 translate: create a translation matrix, 
	    then multiply the transform matrix by the translation matrix - 
	    takes 3 arguments (tx, ty, tz)
	 xrotate: create an x-axis rotation matrix,
	    then multiply the transform matrix by the rotation matrix -
	    takes 1 argument (theta)
	 yrotate: create an y-axis rotation matrix,
	    then multiply the transform matrix by the rotation matrix -
	    takes 1 argument (theta)
	 zrotate: create an z-axis rotation matrix,
	    then multiply the transform matrix by the rotation matrix -
	    takes 1 argument (theta)
	 apply: apply the current transformation matrix to the 
	    edge matrix
	 display: draw the lines of the edge matrix to the screen
	    display the screen
	 save: draw the lines of the edge matrix to the screen
	    save the screen to a file -
	    takes 1 argument (file name)
	 quit: end parsing

See the file script for an example of the file format


IMPORTANT MATH NOTE:
the trig functions int math.h use radian measure, but us normal
humans use degrees, so the file will contain degrees for rotations,
be sure to conver those degrees to radians (M_PI is the constant
for PI)
====================*/
void parse_file ( char * filename, 
                  struct matrix * transform, 
                  struct matrix * pm,
                  screen s) {

  FILE *f;
  char line[256];
  
  color c;
  c.red = MAX_COLOR;
  c.green = 0;
  c.blue = 0;

  clear_screen(s);

  if ( strcmp(filename, "stdin") == 0 ) 
    f = stdin;
  else
    f = fopen(filename, "r");
  
  while ( fgets(line, 255, f) != NULL ) {
    line[strlen(line)-1]='\0';
    printf(":%s:\n",line);  //end of dw code

    if (strcmp(line, "line") == 0) {
      fgets(line, 255, f);
      line[strlen(line) - 1] = '\0';

      printf("LINE: %s\n", line);
      
      char * points[6];
      int i;

      points[0] = strtok(line, " ");
      
      for(i = 0; i < 5; i++)
	points[i+1] = strtok(NULL, " ");

      add_edge(pm, atof(points[0]), atof(points[1]), atof(points[2]), atof(points[3]), atof(points[4]), atof(points[5]));
    }
    else if (strcmp(line, "circle") == 0) {
      fgets(line, 255, f);
      line[strlen(line) - 1] = '\0';

      char * points[3];
      int i;

      points[0] = strtok(line, " ");
      
      for(i = 0; i < 2; i++)
	points[i + 1] = strtok(NULL, " ");

      add_circle(pm, atof(points[0]), atof(points[1]), atof(points[2]), 1000);
    }
    else if (strcmp(line, "hermite") == 0) {
      fgets(line, 255, f);
      line[strlen(line) - 1] = '\0';

      char * points[8];
      int i;
      
      points[0] = strtok(line, " ");
      
      for(i = 0; i < 7; i++)
	points[i + 1] = strtok(NULL, " ");

      printf("HERMITE: %s,%s,%s,%s,%s,%s,%s,%s\n", points[0],points[1],points[2],points[3],points[4],points[5],points[6],points[7]);
      
      add_curve(pm, atof(points[0]), atof(points[1]), atof(points[2]), atof(points[3]), atof(points[4]), atof(points[5]), atof(points[6]), atof(points[7]), 100, HERMITE_MODE);
    }
    else if (strcmp(line, "bezier") == 0) {
      fgets(line, 255, f);
      line[strlen(line) - 1] = '\0';

      char * points[8];
      int i;

      points[0] = strtok(line, " ");
      
      for(i = 0; i < 7; i++)
	points[i + 1] = strtok(NULL, " ");

      add_curve(pm, atof(points[0]), atof(points[1]), atof(points[2]), atof(points[3]), atof(points[4]), atof(points[5]), atof(points[6]), atof(points[7]), 1000, BEZIER_MODE); 
    }
    else if (strcmp(line, "ident") == 0) {
      ident(transform);
    }
    else if (strcmp(line, "translate") == 0) {
      fgets(line, 255, f);
      line[strlen(line) - 1] = '\0';

      char * sX, * sY, * sZ;

      sX = strtok(line, " ");
      sY = strtok(NULL, " ");
      sZ = strtok(NULL, " ");

      transform = make_translate(atof(sX), atof(sY), atof(sZ));
    }
    else if (strcmp(line, "scale") == 0) {
      fgets(line, 255, f);
      line[strlen(line) - 1] = '\0';

      char * sX, * sY, * sZ;

      sX = strtok(line, " ");
      sY = strtok(NULL, " ");
      sZ = strtok(NULL, " ");

      transform = make_scale(atof(sX), atof(sY), atof(sZ));
    }
    else if (strcmp(line, "xrotate") == 0) {
      fgets(line, 255, f);
      line[strlen(line) - 1] = '\0';
      
      matrix_mult(make_rotX(M_PI * atof(line) / 180), transform);
    }
    else if (strcmp(line, "yrotate") == 0) {
      fgets(line, 255, f);
      line[strlen(line) - 1] = '\0';
      
      matrix_mult(make_rotY(M_PI * atof(line) / 180), transform);
    }
    else if (strcmp(line, "zrotate") == 0) {
      fgets(line, 255, f);
      line[strlen(line) - 1] = '\0';
      
      matrix_mult(make_rotZ(M_PI * atof(line) / 180), transform);
    }
    else if (strcmp(line, "apply") == 0) {
      matrix_mult(transform, pm);
    }
    else if (strcmp(line, "display") == 0) {
    	/*
      color c;
      c.red = 255;
      c.green = 150;
      c.blue = 100;
	*/
      draw_lines(pm, s, c);
      display(s);
      clear_screen(s);
    }
    else if (strcmp(line, "save") == 0) {
      save_extension(s, "curves.png");
    }
    else if (strcmp(line, "quit") == 0) {
      exit(0);
    }
  }

 }



  
