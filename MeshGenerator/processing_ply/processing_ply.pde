// Program to load & display vertices & faces in PLY format, 
// then save triangle faces in PLY format
PImage handImg;
import processing.pdf.*;
boolean bRecordPdf;

final int POINT_ADDING_MODE = 0; 
final int FACE_ADDING_MODE  = 1; 
final int POINT_MOVING_MODE = 2; 
final int FACE_REORDERING_MODE = 3;
int mode = POINT_ADDING_MODE; 

String plyFilename = "genericHand.ply"; //handmarks_532_faces.ply"; //handmarks_532_faces.ply";
String loadedPlyFile[]; 
ArrayList <Vertex> vertices;
ArrayList <Face> faces; 

float upScale = 1.0; 


int highlightFaceIndex = -1;
int highlightVertexIndex = -1;
Face possibleFace;
Vertex mouseVertex;

int linkVertexIndices[];

void setup() {

  handImg = loadImage("hand.jpg"); 
  bRecordPdf = false; 
  mode = 0;

  linkVertexIndices = new int[2];
  linkVertexIndices[0] = -1;
  linkVertexIndices[1] = -1;

  vertices = new ArrayList <Vertex>();
  faces    = new ArrayList <Face>();
  possibleFace = new Face(); 
  mouseVertex  = new Vertex();



  loadPlyFile();
  
  size (1024, 768, OPENGL);
}


//======================================================
void draw() {
  
  

  pushMatrix();
  scale(upScale, upScale); 

  if (bRecordPdf) {
    beginRecord(PDF, "handmarks.pdf"); 
    strokeWeight(0.25);
  } 
  else {
    strokeWeight(1.0);
  }


  strokeJoin(ROUND); 

  if (mode == POINT_ADDING_MODE) {
    background(255, 240, 240);
  } 
  else if (mode == FACE_ADDING_MODE) {
    background(240, 255, 240);
  } 
  else if (mode == POINT_MOVING_MODE) {
    background(240, 240, 255);
  }


  image(handImg, 0,0);


  noFill();
  stroke (0);
  for (int i=0; i<vertices.size(); i++) {
    Vertex v = vertices.get(i);
    float x = v.x;
    float y = v.y;
    ellipse (x, y, 6, 6);
  }

  stroke(0, 0, 0, 100); 
  possibleFace.draw (true);

  for (int i=0; i<faces.size(); i++) {
    Face f = faces.get(i);
    if (i == highlightFaceIndex) {
      f.draw(true);
    } 
    else {
      f.draw(false);
    }
  }

  if (bRecordPdf) {
    endRecord();
    bRecordPdf = false;
  }

  if (mode == POINT_ADDING_MODE) {
    fill (255, 240, 0); 
    if (linkVertexIndices[0] != -1) {
      vertices.get(linkVertexIndices[0]).draw();
    } 
    if (linkVertexIndices[1] != -1) {
      vertices.get(linkVertexIndices[1]).draw();
    }
  } 
  else if (mode == FACE_ADDING_MODE) {
    highlightFaceIndex = -1;
  }

  if (mode == POINT_MOVING_MODE) {
    if (mousePressed) {
      mouseVertex.x = mouseX/upScale; 
      mouseVertex.y = mouseY/upScale;
      if (highlightVertexIndex != -1) {
        boolean bEnableMoving = false;
        if (bEnableMoving) {
          vertices.get(highlightVertexIndex).x = mouseVertex.x;
          vertices.get(highlightVertexIndex).y = mouseVertex.y;
        }
        fill (255, 230, 0, 180); 
        Vertex hv = vertices.get(highlightVertexIndex);
        ellipse(hv.x, hv.y, 9, 9);
      }
    }
  }

  boolean bDrawColorIndexed = true;
  if (bDrawColorIndexed) {
    pushMatrix(); 
    translate(512, 0);
    noSmooth();  
 
    int nFaces = min(mouseX, faces.size()); 
    for (int i=0; i<nFaces; i++) {
      Face f = faces.get(i);
      float q = map(i, 0, nFaces-1, 0, 255); 
      fill (color (q, q, 0)); 
      f.drawSimple();
    }
    popMatrix();
  }


  fill (0); 
  float ty = 0; 
  //text("'  ' to add current triangle", 5, ty+=15);
  //text("DEL to kill current triangle", 5, ty+=15);
  //text("'d' to delete most recently added triangle", 5, ty+=15); 
  text("mode =  " + mode, 5, ty+=15);
  //text(mouseVertex.x + ", " + mouseVertex.y, 5, ty+=15);

  popMatrix();
}

//=======================================================
void mousePressed() {
  int whichVertex = -1; 
  int whichFace = -1; 
  mouseVertex.x = mouseX/upScale; 
  mouseVertex.y = mouseY/upScale;

  float minDist = 99999;
  for (int i=0; i<vertices.size(); i++) {
    Vertex v = vertices.get(i);
    float x = v.x;
    float y = v.y;
    float h = dist(x, y, mouseVertex.x, mouseVertex.y); 
    if ((h < minDist) && (h < 20)) {
      minDist = h; 
      whichVertex = i;
    }
  }

  int oldHighlightFaceIndex = highlightFaceIndex;
  highlightFaceIndex = -1; 
  for (int i=0; i<faces. size(); i++) {
    Face f = faces.get(i);
    if (pointInFace( mouseVertex, f)) {
      println ("Clicked on face: " + i);
      if (oldHighlightFaceIndex == i) {
        highlightFaceIndex = -1;
      } 
      else {
        highlightFaceIndex = i;
      }
    }
  }

  //highlightVertexIndex = -1;
  if (whichVertex > -1) {
    if (mode == FACE_ADDING_MODE) {
      possibleFace.add(whichVertex);
    }
  }
  if (mode == POINT_ADDING_MODE) {
    linkVertexIndices[0] = linkVertexIndices[1];
    linkVertexIndices[1] = whichVertex; 
    println ("Link vertices: " + linkVertexIndices[0] + ", " + linkVertexIndices[1]);
  }
  if (mode == POINT_MOVING_MODE) {
    highlightVertexIndex = whichVertex;
    println ("Clicked on vertex: " + highlightVertexIndex);
  }
}


void mouseReleased() {
  highlightVertexIndex = -1;
}

//================================================================
boolean pointInFace( Vertex p, Face f) {

  int ia = f.indices[0]; 
  int ib = f.indices[1]; 
  int ic = f.indices[2];
  if ((ia != -1) && (ib != -1) && (ic != -1)) {
    Vertex a = vertices.get( ia );
    Vertex b = vertices.get( ib );
    Vertex c = vertices.get( ic );

    boolean sideA = ((b.y-a.y)*(p.x-a.x)-(b.x-a.x)*(p.y-a.y)) > 0.0f;
    boolean sideB = ((c.y-b.y)*(p.x-b.x)-(c.x-b.x)*(p.y-b.y)) > 0.0f;
    if (sideA != sideB) 
      return false;
    boolean sideC = ((a.y-c.y)*(p.x-c.x)-(a.x-c.x)*(p.y-c.y)) > 0.0f;
    return (sideA == sideC);
  } 
  return false;
}

//===========================================================
void keyPressed() {
  if (keyCode == 8) { // delete 
    possibleFace.clear();
  } 


  if (key == 'Q') {
    mode = POINT_ADDING_MODE;
  } 
  else if (key == 'W') {
    mode = FACE_ADDING_MODE;
  } 
  else if (key == 'E') {
    mode = POINT_MOVING_MODE;
  }


  if (key == 'd') {
    if (faces.size() > 0) {
      if (mode == FACE_ADDING_MODE) {
        faces.remove(faces.size() - 1);
      } 
      else if (mode == POINT_ADDING_MODE) {
        if ((highlightFaceIndex > -1) && (highlightFaceIndex < faces.size())) {
          faces.remove(highlightFaceIndex);
          highlightFaceIndex = -1;
        }
      }
    }
  }

  if (key == 'P') {
    bRecordPdf = true;
  }
  else if (key == 'X') {
    if (mode == POINT_MOVING_MODE) {
      if ((highlightVertexIndex > -1) && (highlightVertexIndex < vertices.size())) {
        //vertices.remove(highlightVertexIndex);
        //highlightVertexIndex = -1;
        if (mousePressed) {

          // delete any Faces that contain that index.
          boolean bFacesContainHighlightIndex = false; 
          do {  
            bFacesContainHighlightIndex = false;
            for (int i=0; i<faces.size(); i++) {
              Face f = faces.get(i);
              if (f.hasIndex (highlightVertexIndex)) {
                bFacesContainHighlightIndex = true;
                faces.remove(i);
              }
            }
          } 
          while (bFacesContainHighlightIndex);

          // remove the vertex from the vertices
          vertices.remove(highlightVertexIndex);

          // now loop over all the faces. 
          // if any Face contains an index which was higher than highlightVertexIndex, 
          // subtract 1 from that index.
          for (int i=0; i<faces.size(); i++) {
            Face f = faces.get(i);
            f.shiftDownIndicesHigherThanRecentlyDeletedIndex(highlightVertexIndex) ;
          }
          
          
          
        }
      }
    }
  } 
  else if (key == 'S') {
    saveFile();
  }


  if (key == ' ') {
    if (possibleFace.nAdded == 3) {
      Face F = new Face(possibleFace); 
      if (F.isKosher()) {
        faces.add(F);
        saveFile();
      }
      possibleFace.clear();
    }
  }

  if (key == 'p') {
    println("---------------------"); 
    for (int i=0; i<faces.size(); i++) {
      Face f = faces.get(i);
      String s = f.print();
      println (s);
    }
  }

  if (key == '-') {
    if (faces.size() > 0) {
      highlightFaceIndex = (highlightFaceIndex - 1 + faces.size())%faces.size();
      println("highlightFaceIndex = " + highlightFaceIndex);
    }
  } 
  else if (key == '=') {
    if (faces.size() > 0) {
      highlightFaceIndex = (highlightFaceIndex + 1)%faces.size();
      println("highlightFaceIndex = " + highlightFaceIndex);
    }
  }

  if (key == '2') {
    if ((linkVertexIndices[0] != -1) && (linkVertexIndices[1] != -1)) {
      Vertex v0 = vertices.get(linkVertexIndices[0]);
      Vertex v1 = vertices.get(linkVertexIndices[1]);
      float vx = (v0.x + v1.x)/2.0; 
      float vy = (v0.y + v1.y)/2.0; 
      Vertex aNewVertex = new Vertex(vx, vy);
      vertices.add(aNewVertex);
    }
  } 
  else if (key == '3') {
    if ((linkVertexIndices[0] != -1) && (linkVertexIndices[1] != -1)) {
      Vertex v0 = vertices.get(linkVertexIndices[0]);
      Vertex v1 = vertices.get(linkVertexIndices[1]);
      float vxa = (2.0*v0.x + v1.x)/3.0; 
      float vya = (2.0*v0.y + v1.y)/3.0; 
      float vxb = (v0.x + 2.0*v1.x)/3.0; 
      float vyb = (v0.y + 2.0*v1.y)/3.0; 
      Vertex aNewVertex = new Vertex(vxa, vya);
      Vertex bNewVertex = new Vertex(vxb, vyb);
      vertices.add(aNewVertex);
      vertices.add(bNewVertex);
    }
  }
  else if (key == '4') {
    if ((linkVertexIndices[0] != -1) && (linkVertexIndices[1] != -1)) {
      Vertex v0 = vertices.get(linkVertexIndices[0]);
      Vertex v1 = vertices.get(linkVertexIndices[1]);
      for (int i=1; i<=3; i++) {
        float vx = ((4-i)*v0.x + (i)*v1.x)/4.0; 
        float vy = ((4-i)*v0.y + (i)*v1.y)/4.0; 
        Vertex aNewVertex = new Vertex(vx, vy);
        vertices.add(aNewVertex);
      }
    }
  }
}




float crossProduct (Vertex A, Vertex B, Vertex C) {
  float BAdx = B.x - A.x; 
  float BAdy = B.y - A.y; 

  float CAdx = C.x - A.x; 
  float CAdy = C.y - A.y; 

  float out = (BAdx*CAdy)-(BAdy*CAdx);
  return out;
}

