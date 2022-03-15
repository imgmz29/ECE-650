#include "exerciser.h"

void exercise(connection *C){  
  query1(C, 1, 35, 40, 0, 0, 0, 0, 5, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  query1(C, 0, 35, 40, 0, 0, 0, 1, 5, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  query2(C, "DarkBlue");
  query2(C, "Red");
  query3(C, "Duke");
  query3(C, "BostonCollege");
  query4(C, "MA", "Maroon");
  query4(C, "NC", "DarkBlue");
  query5(C, 5);
  query5(C, 10);
}
