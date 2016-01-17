/**
 * @file SchedulerNamespaces.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) Mikael Patel, 2015
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * @section Description
 * This Arduino sketch shows how to use the Scheduler library.
 * Rewrite of the SchedulerDemo sketch using namespaces for
 * each task.
 */

#include <Scheduler.h>

namespace BlinkTask {

  const unsigned int PERIOD = 1000;
  const int LED = 13;

  void setup()
  {
    Serial.print(millis());
    Serial.println(F(":BlinkTask::setup"));
    pinMode(LED, OUTPUT);
  }

  void loop()
  {
    // Turn LED off
    Serial.print(millis());
    Serial.println(F(":BlinkTask::loop:led off"));
    digitalWrite(LED, LOW);
    delay(PERIOD);

    // Turn LED on
    Serial.print(millis());
    Serial.println(F(":BlinkTask::loop:led on"));
    digitalWrite(LED, HIGH);
    delay(PERIOD);
  }
};

namespace ShellTask {

  const size_t BUF_MAX = 64;
  static char* buf = NULL;

  void setup()
  {
    Serial.print(millis());
    Serial.println(F(":ShellTask::setup"));
  }

  void loop()
  {
    // Check for buffer allocation
    if (buf == NULL) {
      buf = (char*) malloc(BUF_MAX);
      Serial.print(millis());
      Serial.print(F(":ShellTask::loop:alloc:buf=0x"));
      Serial.println((int) buf, HEX);
      if (buf == NULL) {
	delay(1000);
	return;
      }
    }

    // Read line and yield while waiting for characters
    // Capture wait time and number of yields
    char* bp = buf;
    unsigned long yields = 0;
    unsigned long start = millis();
    int c;
    while ((c = Serial.read()) != '\n') {
      if (c > 0)
	*bp++ = c;
      else {
	yields += 1;
	yield();
      }
    }
    *bp = 0;

    // Print wait time, number of yields and line
    unsigned long stop = millis();
    unsigned long ms = stop - start;
    Serial.print(millis());
    Serial.print(F(":ShellTask::loop:yields="));
    Serial.print(yields);
    Serial.print(F(",ms="));
    Serial.print(ms);
    Serial.print(F(",buf="));
    Serial.println(buf);

    // Check for buffer free command
    if (!strcmp_P(buf, (const char*) F("free"))) {
      Serial.print(millis());
      Serial.print(F(":ShellTask::loop:free:buf=0x"));
      Serial.println((int) buf, HEX);
      free(buf);
      delay(500);
      buf = NULL;
    }
  }
};


void setup()
{
  Serial.begin(57600);
  Serial.println(F("SchedulerNamespaces: started"));

  // Initiate tasks
  Scheduler.begin();
  Scheduler.start(BlinkTask::setup, BlinkTask::loop);
  Scheduler.start(ShellTask::setup, ShellTask::loop);
}

void loop()
{
  // Main loop iteraction count
  static int i = 0;

  // Print main loop iterations
  Serial.print(millis());
  Serial.print(F(":loop::i="));
  Serial.println(i++);
  delay(500);
}
