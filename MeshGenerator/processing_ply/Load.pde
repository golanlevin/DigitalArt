void loadPlyFile() {


  loadedPlyFile = loadStrings(plyFilename); 
  String stringArray[] = loadedPlyFile; 
  int nStrings = stringArray.length; 

  boolean bFinishedHeader = false;
  int nVertices = 0; 
  int nFaces = 0; 
  int vertexDataStartLineIndex = -1; 
  int vertexDataEndLineIndex   = -1;
  int faceDataStartLineIndex   = -1;

  float maxX = 0; 
  float maxY = 0; 

  for (int i=0; i<nStrings; i++) {
    String aLine = stringArray[i];

    if (aLine.startsWith("element vertex")) {
      String linePieces[] = split(aLine, " "); 
      if (linePieces.length == 3) {
        String nVerticesString = linePieces[2];
        nVertices = Integer.parseInt(nVerticesString);
      }
    }

    if (aLine.startsWith("element face")) {
      String linePieces[] = split(aLine, " "); 
      if (linePieces.length == 3) {
        String nFacesString = linePieces[2];
        nFaces = Integer.parseInt(nFacesString);
      }
    }

    if (aLine.startsWith("end_header")) {
      bFinishedHeader = true;
      vertexDataStartLineIndex = i+1;
      if (nVertices > 0) {
        vertexDataEndLineIndex = vertexDataStartLineIndex + nVertices;
        faceDataStartLineIndex = vertexDataEndLineIndex;
      }
    }

    if (bFinishedHeader) {
      if ((i >= vertexDataStartLineIndex) && (i < vertexDataEndLineIndex)) {
        String linePieces[] = split(aLine, " "); 
        if (linePieces.length >= 2) {

          float x = Float.parseFloat(linePieces[0]);
          float y = Float.parseFloat(linePieces[1]);

          Vertex aNewVertex = new Vertex(x, y);
          vertices.add(aNewVertex);

          if (x > maxX) maxX = x; 
          if (y > maxY) maxY = y;
        }
      }

      if (i >= faceDataStartLineIndex) {
        String linePieces[] = split(aLine, " "); 
        if (linePieces.length == 4) {
          int a = Integer.parseInt(linePieces[1]);
          int b = Integer.parseInt(linePieces[2]);
          int c = Integer.parseInt(linePieces[3]);

          Face aNewFace = new Face(a, b, c);
          faces.add(aNewFace);
        }
      }
    }
  }
}


//================================================================
void saveFile () {
  PrintWriter output;

  String newFilename = year() + nf(month(), 2) + nf(day(), 2); 
  newFilename += "_" + nf(hour(), 2) + nf(minute(), 2) + nf(second(), 2) + "_";
  newFilename += plyFilename; 

  output = createWriter("data/" + newFilename); 

  output.println ("ply"); 
  output.println ("format ascii 1.0");            
  output.println ("comment Made by Golan Levin et al.");   
  output.println ("comment This file is the bind pose for a generic hand."); 
  output.println ("element vertex " + vertices.size()); 
  output.println ("property float x"); 
  output.println ("property float y"); 
  output.println ("element face " + faces.size()); 
  output.println ("property list uchar int vertex_index"); 
  output.println ("end_header"); 

  for (int i=0; i<vertices.size(); i++) {
    Vertex v = vertices.get(i);
    String s = v.print();
    output.println (s);
  }

  for (int i=0; i<faces.size(); i++) {
    Face f = faces.get(i);
    String s = f.print();
    output.println (s);
  }
  output.flush(); // Writes the remaining data to the file
  output.close(); // Finishes the file
}

void printVertices() {
  for (int i=0; i<vertices.size(); i++) {
    Vertex v = vertices.get(i);
    String s = v.print();
    println (i + "\t" + s);
  }
}

