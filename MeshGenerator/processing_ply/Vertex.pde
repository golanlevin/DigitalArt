class Vertex {
  float x;
  float y;
  
  Vertex(){
    x = 0; 
    y = 0; 
  }
  
  Vertex (float inx, float iny) {
    x = inx; 
    y = iny;
  }
  
  String print(){
    String out = nf(x, 1,3) + " " + nf(y, 1,3);
    return out;
  }
  
  void draw(){
    ellipse (x, y, 9,9);
  }
}

