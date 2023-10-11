int x = 0;
float y_sin = 0.0;
float y_cos = 0.0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  x = x + 1;
  y_sin = sin(x * 0.1);  
  y_cos = cos(x * 0.1);  
  
  y_sin = y_sin * 2 - 1;
  y_cos = y_cos * 2 - 1;

 
  int y_sin_result = (y_sin + 1) * 127;
  int y_cos_result = (y_cos + 1) * 127;

  Serial.printf("%d %d %d\n", 0, y_sin_result, y_cos_result);
  delay(10); 
}
