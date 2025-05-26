// Include the Servo library to control the servo motor
#include <Servo.h> 

// Include SPI library and Ucglib for the display
#include <SPI.h>
#include "Ucglib.h"

// Define pins for the ultrasonic sensor
const int trigPin = 6;
const int echoPin = 5;

// Display dimensions
int Ymax = 128;
int Xmax = 160;

// Display drawing base and servo positioning
int base = 8;
int pos = base + 6;

// Variables for degree mapping and drawing logic
int deg = 0;
int x;
int val = 200; // Line length value
int j = 2; // Unused but declared
Servo myServo; // Create servo object

// Variables for distance measurement
long duration;
int distance;
int k; // Servo angle

// Initialize the display object with SPI communication
Ucglib_ST7735_18x128x160_HWSPI ucg(/cd=/ 9, /cs=/ 10, /reset=/ 8);

void setup(void)
{
  delay(1000); // Give components time to power up
  myServo.write(80); // Set initial servo position

  // Initialize the display in solid font mode
  ucg.begin(UCG_FONT_MODE_SOLID);
  ucg.setFont(ucg_font_6x10_tr); // Set font
  ucg.clearScreen(); // Clear display
  ucg.setRotate270(); // Rotate screen for vertical orientation

  pinMode(trigPin, OUTPUT); // Set trig pin as output for ultrasonic
  pinMode(echoPin, INPUT);  // Set echo pin as input for ultrasonic
  Serial.begin(9600); // Begin serial communication
  myServo.attach(3); // Attach servo to pin 3
}

void loop(void)
{
  // Draw background (radar grid and center)
  fix(); 

  // Sweep servo from right (80°) to left (10°)
  for (x = 80; x >= 10; x--) {  
    distance = calculateDistance(); // Get distance from ultrasonic sensor
    Serial.println(distance); // Print to Serial Monitor

    k = map(x, 80, 10, 15, 165); // Map sweep angle to servo angle
    myServo.write(k); // Move servo

    // If object is within 30 cm, draw red detection line
    if (distance < 30) {
      int f = x + 6;
      ucg.setColor(255, 0, 0);
      ucg.drawLine(Xmax / 2, pos, -val * cos(radians(f * 2)), val * sin(radians(f * 2)));
    }

    // Draw radar sweep line in green
    ucg.setColor(0, 207, 0);
    ucg.drawLine(Xmax / 2, pos, -200 * cos(radians(x * 2)), 200 * sin(radians(x * 2)));

    // Past sweep lines in lighter/dimmer shades for trail effect
    int d = x + 1;
    int c = x + 2;
    int b = x + 3;
    int a = x + 4;
    int e = x + 5;

    ucg.setColor(0, 0, 0); // Erase the oldest sweep line
    ucg.drawLine(Xmax / 2, pos, -200 * cos(radians(e * 2)), 200 * sin(radians(e * 2)));

    // Display current angle and distance
    ucg.setColor(255, 0, 0);
    ucg.setPrintPos(160, 0);
    ucg.setPrintDir(2);
    ucg.print("Deg :");

    deg = map(x, 80, 10, 0, 180); // Map x value to degree for display
    ucg.setPrintPos(120, 0);
    ucg.print(deg);

    ucg.setPrintPos(10, 0);
    ucg.print(distance);

    // Show scale markers on the display
    ucg.setColor(0, 0, 255);
    ucg.setPrintPos(90, 38);
    ucg.print("0.25");
    ucg.setPrintPos(90, 70);
    ucg.print("0.50");
    ucg.setPrintPos(90, 110);
    ucg.print("1.00");
  }

  fix(); // Redraw the radar background

  // Sweep back from left (10°) to right (80°)
  for (x = 10; x <= 80; x++) {
    distance = calculateDistance();
    Serial.println(distance);

    k = map(x, 10, 80, 165, 15); // Reverse sweep angle mapping
    myServo.write(k);

    if (distance < 10) {
      int e = x - 5;
      ucg.setColor(255, 0, 0);
      ucg.drawLine(Xmax / 2, pos, -val * cos(radians(e * 2)), val * sin(radians(e * 2)));
    }

    ucg.setColor(0, 207, 0);
    ucg.drawLine(Xmax / 2, pos, -200 * cos(radians(x * 2)), 200 * sin(radians(x * 2)));

    // Older trails for radar sweep (some commented out)
    int a = x - 1;
    int b = x - 2;
    int c = x - 3;
    int d = x - 4;

    ucg.setColor(0, 0, 0); // Clear oldest trail
    ucg.drawLine(Xmax / 2, pos, -200 * cos(radians(d * 2)), 200 * sin(radians(d * 2)));

    // Display angle and distance
    ucg.setColor(255, 0, 0);
    ucg.setPrintPos(160, 0);
    ucg.setPrintDir(2);
    ucg.print("Deg :");

    deg = map(x, 10, 80, 0, 180);
    ucg.setPrintPos(120, 0);
    ucg.print(deg);

    ucg.setPrintPos(10, 0);
    ucg.print(distance);

    // Scale markers
    ucg.setColor(0, 0, 255);
    ucg.setPrintPos(90, 38);
    ucg.print("0.25");
    ucg.setPrintPos(90, 70);
    ucg.print("0.50");
    ucg.setPrintPos(90, 110);
    ucg.print("1.00");
  }
}

// Function to draw the radar base and arcs
void fix() {
  // Draw the radar’s center point (dual red arcs)
  ucg.setColor(255, 0, 0);
  ucg.drawDisc(Xmax / 2, base, 5, UCG_DRAW_LOWER_RIGHT);
  ucg.drawDisc(Xmax / 2, base, 5, UCG_DRAW_LOWER_LEFT);

  // Draw radar concentric circles
  ucg.setColor(225, 255, 50);
  ucg.drawCircle(80, base, 115, UCG_DRAW_LOWER_RIGHT);
  ucg.drawCircle(80, base, 115, UCG_DRAW_LOWER_LEFT);

  ucg.drawCircle(80, base, 78, UCG_DRAW_LOWER_RIGHT);
  ucg.drawCircle(80, base, 78, UCG_DRAW_LOWER_LEFT);

  ucg.drawCircle(80, base, 40, UCG_DRAW_LOWER_RIGHT);
  ucg.drawCircle(80, base, 40, UCG_DRAW_LOWER_LEFT);

  // Draw the horizontal baseline
  ucg.drawLine(0, base, 160, base);

  // Clear old print area (top bar) to avoid overlapping numbers
  ucg.setColor(0, 0, 0);
  ucg.drawBox(100, 0, 30, 8);
}

// Function to measure distance using ultrasonic sensor
int calculateDistance() {
  digitalWrite(trigPin, LOW); // Clear trigPin
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH); // Send 10us pulse
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read echoPin to get pulse duration
  duration = pulseIn(echoPin, HIGH);

  // Convert time to distance (cm)
  distance = duration * 0.034 / 2;

  return distance;
}
