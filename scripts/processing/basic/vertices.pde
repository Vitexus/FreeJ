size(200, 200);
background(0);
noFill();

stroke(102);
beginShape();
curveVertex(168, 182);
curveVertex(168, 182);
curveVertex(136, 38);
curveVertex(42, 34);
curveVertex(64, 200);
curveVertex(64, 200);
endShape();

stroke(51);
beginShape(LINES);
vertex(60, 40);
vertex(160, 10);
vertex(170, 150);
vertex(60, 150);
endShape();

stroke(126);
beginShape();
vertex(60, 40);
bezierVertex(160, 10, 170, 150, 60, 150);
endShape();

stroke(255);
beginShape(POINTS);
vertex(60, 40);
vertex(160, 10);
vertex(170, 150);
vertex(60, 150);
endShape();
