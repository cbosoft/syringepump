#include <Arduino.h>
#include "log.h"

extern int log_options;

const int log_time = 1 << 5;
const int log_force = 1 << 4;
const int log_flow = 1 << 3;
const int log_control_action = 1 << 2;
const int log_loadcell = 1 << 1;
const int log_ticks = 1;

void logToSerial(
    long time, double force, double flowrate, 
    double control_action, long load_cell_reading, unsigned long ticks)
{
  int ncols = 0;

  if (log_options & log_time) {
    Serial.print(time);
    ncols ++;
  }

  if (log_options & log_force) {
    if (ncols) Serial.print(",");
    Serial.print(force, 5);
  }

  if (log_options & log_flow) {
    if (ncols) Serial.print(",");
    Serial.print(flowrate, 5);
  }

  if (log_options & log_control_action) {
    if (ncols) Serial.print(",");
    Serial.print(long(control_action));
  }

  if (log_options & log_loadcell) {
    if (ncols) Serial.print(",");
    Serial.print(load_cell_reading);
  }

  if (log_options & log_ticks) {
    if (ncols) Serial.print(",");
    Serial.print(ticks);
  }

  Serial.print("\n");
}

void logTitlesToSerial()
{
  int ncols = 0;

  if (log_options & log_time) {
    Serial.print("Time (ms)");
    ncols ++;
  }

  if (log_options & log_force) {
    if (ncols) Serial.print(",");
    Serial.print("Force (N)");
  }

  if (log_options & log_flow) {
    if (ncols) Serial.print(",");
    Serial.print("Flowrate (ml/s)");
  }

  if (log_options & log_control_action) {
    if (ncols) Serial.print(",");
    Serial.print("Control action (8 bit)");
  }

  if (log_options & log_loadcell) {
    if (ncols) Serial.print(",");
    Serial.print("Loadcell (24 bit)");
  }

  if (log_options & log_ticks) {
    if (ncols) Serial.print(",");
    Serial.print("Ticks (rotations)");
  }

  Serial.print("\n");
}

// vim: ft=arduino
