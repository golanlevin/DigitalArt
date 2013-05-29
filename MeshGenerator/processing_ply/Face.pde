class Face {
  int indices[];
  int nAdded = 0; 

  Face() {
    indices = new int[3]; 
    clear();
  }
  
  

  Face (int ina, int inb, int inc) {
    indices = new int[3]; 
    indices[0] = ina; 
    indices[1] = inb; 
    indices[2] = inc; 
    nAdded = 3;
  }

  Face (Face otherFace) {
    indices = new int[3]; 
    indices[0] = otherFace.indices[0]; 
    indices[1] = otherFace.indices[1];  
    indices[2] = otherFace.indices[2]; 
    nAdded = otherFace.nAdded;
  }

  void clear() { 
    indices[0] = indices[1] = indices[2] = -1; 
    nAdded = 0;
  }
  void add (int index) {
    if (nAdded < 3) {
      indices[nAdded] = index;
      nAdded++;
    }
  }

  String print() {
    String out = ""; 
    if (nAdded == 3) {
      int a = indices[0]; 
      int b = indices[1]; 
      int c = indices[2];
      if ((a != -1) && (b != -1) && (c != -1)) {
        out = "3 " + a + " " + b + " " + c;
      }
    }
    return out;
  }
  
  boolean hasIndex (int which){
    if ((indices[0] == which) || (indices[1] == which) || (indices[2] == which)){
      return true; 
    }
    return false;
  }
  
  void shiftDownIndicesHigherThanRecentlyDeletedIndex( int which){
    if (indices[0] > which) {
      indices[0]--;
    }
    if (indices[1] > which) {
      indices[1]--;
    }
    if (indices[2] > which) {
      indices[2]--;
    }
  }

  boolean isKosher() {
    boolean out = false;
    int a = indices[0]; 
    int b = indices[1]; 
    int c = indices[2];
    if (nAdded == 3) {
      if ((a != -1) && (b != -1) && (c != -1)) {
        Vertex va = vertices.get( a );
        Vertex vb = vertices.get( b );
        Vertex vc = vertices.get( c );

        float cross = crossProduct (va, vb, vc); 
        if (cross > 0) {
          out = true;
        }
      }
    }
    return out;
  }



  void draw (boolean special) {

    int a = indices[0]; 
    int b = indices[1]; 
    int c = indices[2];

    if (special && (mode == FACE_ADDING_MODE)) {
      for (int i=0; i<3; i++) {
        if (indices[i] != -1) {  
          Vertex v = vertices.get(indices[i]);
          fill (255, 0, 0); 
          ellipse(v.x, v.y, 6, 6);
        }
      }
    }

    if ((nAdded == 3) && (vertices.size() >= 3)) {
      if ((a != -1) && (b != -1) && (c != -1)) {
        Vertex va = vertices.get( a );
        Vertex vb = vertices.get( b );
        Vertex vc = vertices.get( c );

        if (special) {
          float cross = crossProduct (va, vb, vc); 
          if (cross < 0) {
            fill (255, 50, 50, 128);
          } 
          else {
            fill ( 60, 200, 100, 128);
          }
        } 
        else {
          fill (0, 0, 0, 50);
        }
        stroke (0); 
        triangle (va.x, va.y, vb.x, vb.y, vc.x, vc.y);
      }
    }
  }



  void drawSimple() {

    if ((nAdded == 3) && (vertices.size() >= 3)) {
      int a = indices[0]; 
      int b = indices[1]; 
      int c = indices[2];

      if ((a != -1) && (b != -1) && (c != -1)) {
        Vertex va = vertices.get( a );
        Vertex vb = vertices.get( b );
        Vertex vc = vertices.get( c ); 
        triangle (va.x, va.y, vb.x, vb.y, vc.x, vc.y);
      }
    }
  }
  
}

