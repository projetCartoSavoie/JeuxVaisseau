#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <assert.h>
#include "geometry.h"
#include "array.h"
#include "triangulation_tools.h"
#include "tiny_gc.h"
#include "tests.h"


half_edge test_tetra() {
  size_t n = 4;
  gl_vertex *v = GC_malloc(n * sizeof(gl_vertex));

  v[0] = *(GLvertex3d(-1.0,-1.0,-1.0));
  v[1] = *(GLvertex3d(1.0,0.0,0.0));
  v[2] = *(GLvertex3d(0.0,1.0,0.0));
  v[3] = *(GLvertex3d(0.0,0.0,1.0));

  return tetraedron(&v[1],&v[0],&v[2],&v[3]);
}
