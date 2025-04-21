import processing.net.*;
Client client;

// Orlando Graph Visualizer 

Table edges;
HashMap<String, PVector> nodes = new HashMap<String, PVector>();
ArrayList<Edge> graphEdges = new ArrayList<Edge>();
ArrayList<PVector> dijkstraPath = new ArrayList<PVector>();
ArrayList<PVector> aStarPath = new ArrayList<PVector>();
HashMap<String, Integer> nodeDegree = new HashMap<String, Integer>();
HashMap<PVector, String> realCoords = new HashMap<PVector, String>();

float zoom = 1.0;
float offsetX = 0, offsetY = 0;
float prevMouseX, prevMouseY;
boolean dragging = false;

PVector startNode = null;
PVector goalNode = null;
boolean showAStar = false;
boolean showDijkstra = false;

float minX = Float.MAX_VALUE, maxX = -Float.MAX_VALUE;
float minY = Float.MAX_VALUE, maxY = -Float.MAX_VALUE;

HashMap<String, PVector> touristSpots = new HashMap<String, PVector>();

void settings() {
  fullScreen();
}

void setup() {
  edges = loadTable("orlando_graph1.csv", "header");

  for (TableRow row : edges.rows()) {
    float x1 = row.getFloat("x1");
    float y1 = row.getFloat("y1");
    float x2 = row.getFloat("x2");
    float y2 = row.getFloat("y2");

    PVector p1 = new PVector(x1, y1);
    PVector p2 = new PVector(x2, y2);

    String key1 = x1 + "," + y1;
    String key2 = x2 + "," + y2;

    nodeDegree.put(key1, nodeDegree.getOrDefault(key1, 0) + 1);
    nodeDegree.put(key2, nodeDegree.getOrDefault(key2, 0) + 1);

    minX = min(minX, min(x1, x2));
    maxX = max(maxX, max(x1, x2));
    minY = min(minY, min(y1, y2));
    maxY = max(maxY, max(y1, y2));

    nodes.put(key1, p1);
    nodes.put(key2, p2);
    realCoords.put(p1, key1);
    realCoords.put(p2, key2);

    graphEdges.add(new Edge(p1, p2));
  }

  graphEdges.removeIf(e -> isIsolated(e.a) || isIsolated(e.b));
  snapTouristSpotsToNearestNodes();

  float centerX = map((minX + maxX) / 2, minX, maxX, -width/2, width/2);
  float centerY = map((minY + maxY) / 2, minY, maxY, height/2, -height/2);
  offsetX = width / 2 - centerX;
  offsetY = height / 2 - centerY;

  zoom = 1.0;
}

boolean isIsolated(PVector p) {
  String key = p.x + "," + p.y;
  return nodeDegree.getOrDefault(key, 0) <= 1;
}

void draw() {
  background(250, 240, 210);  
  translate(offsetX, offsetY);
  scale(zoom);

  stroke(180);
  strokeWeight(1.2);
  for (Edge e : graphEdges) {
    e.display();
  }

  drawNodes();
  drawTouristLabels();

  if (showDijkstra) animatePath(dijkstraPath, color(135, 206, 250));
  if (showAStar) animatePath(aStarPath, color(255, 80, 150));

  if (client != null && client.available() > 0) {
    String response = client.readStringUntil('\n');
    if (response != null) {
      println("Response from C++: " + response);
      parsePaths(response);
      client.stop();
      client = null;
    }
  }
}

void drawNodes() {
  for (PVector p : nodes.values()) {
    PVector screen = toScreen(p);
    float size = 5;

    if (startNode != null && p == startNode) {
      fill(0, 255, 0);
      size = 10;
    } else if (goalNode != null && p == goalNode) {
      fill(255, 0, 0);
      size = 10;
    } else {
      fill(150, 180, 220); 
    }

    stroke(20, 40, 100);
    strokeWeight(1.2);
    ellipse(screen.x, screen.y, size, size);
  }
}

void drawTouristLabels() {
  textSize(14);
  for (String name : touristSpots.keySet()) {
    PVector loc = touristSpots.get(name);
    PVector screen = toScreen(loc);
    String label = name;

    float padding = 4;
    float w = textWidth(label) + padding * 2;
    float h = textAscent() + textDescent() + padding * 2;

    stroke(0);
    strokeWeight(1.5);
    fill(20, 40, 100);
    rect(screen.x + 5, screen.y - h + 5, w, h, 6);

    noStroke();
    fill(255);
    text(label, screen.x + 5 + padding, screen.y + 5 - padding);
  }
}

PVector toScreen(PVector p) {
  float sx = map(p.x, minX, maxX, -width/2, width/2);
  float sy = map(p.y, minY, maxY, height/2, -height/2);
  return new PVector(sx, sy);
}

void mousePressed() {
  dragging = true;
  prevMouseX = mouseX;
  prevMouseY = mouseY;

  if (mouseButton == LEFT) {
    PVector worldClick = new PVector((mouseX - offsetX)/zoom, (mouseY - offsetY)/zoom);
    PVector nearest = findNearestNode(worldClick);
    if (nearest != null) {
      if (startNode == null) {
        startNode = nearest;
      } else if (goalNode == null) {
        goalNode = nearest;
        sendToCppServer(startNode, goalNode);
      } else {
        startNode = nearest;
        goalNode = null;
        dijkstraPath.clear();
        aStarPath.clear();
      }
    }
  }
}

void mouseDragged() {
  if (dragging) {
    offsetX += mouseX - prevMouseX;
    offsetY += mouseY - prevMouseY;
    prevMouseX = mouseX;
    prevMouseY = mouseY;
  }
}

void mouseReleased() {
  dragging = false;
}

PVector findNearestNode(PVector worldClick) {
  float threshold = 30;
  PVector best = null;
  float minDist = Float.MAX_VALUE;
  for (PVector p : nodes.values()) {
    PVector screen = toScreen(p);
    float scaledMouseX = (mouseX - offsetX) / zoom;
    float scaledMouseY = (mouseY - offsetY) / zoom;
    float d = dist(scaledMouseX, scaledMouseY, screen.x, screen.y);
    if (d < threshold && d < minDist) {
      minDist = d;
      best = p;
    }
  }
  return best;
}

void animatePath(ArrayList<PVector> path, int col) {
  for (int w = 10; w >= 2; w -= 2) {
    stroke(red(col), green(col), blue(col), 20);
    strokeWeight(w);
    noFill();
    for (int i = 1; i < path.size(); i++) {
      PVector a = toScreen(path.get(i - 1));
      PVector b = toScreen(path.get(i));
      line(a.x, a.y, b.x, b.y);
    }
  }

  stroke(col);
  strokeWeight(4);
  noFill();
  for (int i = 1; i < path.size(); i++) {
    PVector a = toScreen(path.get(i - 1));
    PVector b = toScreen(path.get(i));
    line(a.x, a.y, b.x, b.y);
  }

  if (!path.isEmpty()) {
    PVector tip = toScreen(path.get(path.size() - 1));
    noStroke();
    for (int r = 16; r >= 6; r -= 2) {
      fill(red(col), green(col), blue(col), 30);
      ellipse(tip.x, tip.y, r, r);
    }
    fill(col);
    ellipse(tip.x, tip.y, 8, 8);
  }
}

void mouseWheel(MouseEvent event) {
  float e = event.getCount();
  zoom *= (1 - e * 0.05);
}

void keyPressed() {
  if (key == 'd') {
    showDijkstra = !showDijkstra;
    showAStar = false;
  }
  if (key == 'a') {
    showAStar = !showAStar;
    showDijkstra = false;
  }
}

void sendToCppServer(PVector start, PVector goal) {
  String startCoord = realCoords.get(start);
  String goalCoord = realCoords.get(goal);
  String msg = startCoord + ";" + goalCoord;
  client = new Client(this, "127.0.0.1", 8081);  //port!!
  client.write(msg + "\n");
}

void parsePaths(String data) {
  String[] parts = data.split("\\|");
  dijkstraPath.clear();
  aStarPath.clear();

  for (String part : parts) {
    if (part.startsWith("D:")) {
      String[] coords = part.substring(2).split(";");
      for (String c : coords) {
        if (c.trim().length() > 0) {
          String[] xy = c.split(",");
          float x = float(xy[0]);
          float y = float(xy[1]);
          dijkstraPath.add(new PVector(x, y));
          minX = min(minX, x);
          maxX = max(maxX, x);
          minY = min(minY, y);
          maxY = max(maxY, y);
        }
      }
    } else if (part.startsWith("A:")) {
      String[] coords = part.substring(2).split(";");
      for (String c : coords) {
        if (c.trim().length() > 0) {
          String[] xy = c.split(",");
          float x = float(xy[0]);
          float y = float(xy[1]);
          aStarPath.add(new PVector(x, y));
          minX = min(minX, x);
          maxX = max(maxX, x);
          minY = min(minY, y);
          maxY = max(maxY, y);
        }
      }
    }
  }

  float centerX = map((minX + maxX) / 2, minX, maxX, -width/2, width/2);
  float centerY = map((minY + maxY) / 2, minY, maxY, height/2, -height/2);
  offsetX = width / 2 - centerX;
  offsetY = height / 2 - centerY;

  showDijkstra = true;
  showAStar = true;
}

void snapTouristSpotsToNearestNodes() {
  String[] names = {
    "Universal Studios", "SeaWorld", "Animal Kingdom", "Harry Potter World",
    "Hollywood Studios", "EPCOT", "Magic Kingdom", "Aquatica",
    "Fun Spot America", "Legoland", "Crayola", "ICON Park", "Orlando Eye"
  };
  int step = nodes.size() / names.length;
  int index = 0;
  int count = 0;
  for (String key : nodes.keySet()) {
    if (count % step == 0 && index < names.length) {
      touristSpots.put(names[index], nodes.get(key));
      index++;
    }
    count++;
  }
}

class Edge {
  PVector a, b;
  Edge(PVector a, PVector b) {
    this.a = a;
    this.b = b;
  }
  float length() {
    return PVector.dist(a, b);
  }
  void display() {
    PVector sa = toScreen(a);
    PVector sb = toScreen(b);
    line(sa.x, sa.y, sb.x, sb.y);
  }
}
