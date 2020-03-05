#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>


#include "form.h"
#include "error.h"
#include "util.h"
#include "packet.h"


#define IS_EMPTY_ENTRY(INP) (strlen(gtk_entry_get_text(GTK_ENTRY(INP))) == 0)
#define CHECK_ENTRY_NUMBER(DATA,MESG_EMPTY,MESG_NOT_NUM) \
  if (IS_EMPTY_ENTRY(DATA)) { \
      rv = 1; \
      timestamp_error(data, 1, 0, MESG_EMPTY); \
    } \
    else { \
      if (is_not_number( gtk_entry_get_text(GTK_ENTRY(DATA)) )) { \
        rv = 1; \
        timestamp_error(data, 1, 0, MESG_NOT_NUM); \
      } \
    }


int check_form(struct Data *data)
{
  int rv = 0;

  // check over the form, pre connection.
  // none of the inputs should be empty

  // TODO check tuning parameters and meas time input

  switch (form_get_setter_type(data)) {

    case FORM_SETTER_CONSTANT:
      CHECK_ENTRY_NUMBER(get_object_safe(data, "entA"),
        "C is a required parameter for constant setter",
        "C must be a number (containing only numbers 0-9 and decimal points "
        "('.').");
      break;

    case FORM_SETTER_RAMP:
      CHECK_ENTRY_NUMBER(get_object_safe(data, "entA"),
        "Gradient (M) is a required parameter for ramp setter",
        "Gradient (M) must be a number (containing only numbers 0-9 and decimal points "
        "('.').");
      CHECK_ENTRY_NUMBER(get_object_safe(data, "entB"),
        "Intercept (C) is a required parameter for ramp setter",
        "Intercept (C) must be a number (containing only numbers 0-9 and decimal points "
        "('.').");
      break;

    case FORM_SETTER_STEP:
      CHECK_ENTRY_NUMBER(get_object_safe(data, "entA"),
        "Initial value (I) is a required parameter for step setter",
        "Initial value (I) must be a number (containing only numbers 0-9 and decimal points "
        "('.').");
      CHECK_ENTRY_NUMBER(get_object_safe(data, "entB"),
        "Time of change (tc) is a required parameter for step setter",
        "Time of change (tc) must be a number (containing only numbers 0-9 and decimal points "
        "('.').");
      CHECK_ENTRY_NUMBER(get_object_safe(data, "entC"),
        "Final value (F) is a required parameter for step setter",
        "Final value (F) must be a number (containing only numbers 0-9 and decimal points "
        "('.').");
      break;

    case FORM_SETTER_SINE:
      CHECK_ENTRY_NUMBER(get_object_safe(data, "entA"),
        "Frequency (ω) is a required parameter for sine wave setter",
        "Frequency (ω) must be a number (containing only numbers 0-9 and decimal points "
        "('.').");
      CHECK_ENTRY_NUMBER(get_object_safe(data, "entB"),
        "Magnitude (M) is a required parameter for sine wave setter",
        "Magnitude (M) must be a number (containing only numbers 0-9 and decimal points "
        "('.').");
      CHECK_ENTRY_NUMBER(get_object_safe(data, "entC"),
        "Mean (A) is a required parameter for sine wave setter",
        "Mean (A) must be a number (containing only numbers 0-9 and decimal points "
        "('.').");
      break;

    case FORM_SETTER_SQUARE:
      CHECK_ENTRY_NUMBER(get_object_safe(data, "entA"),
        "Frequency (ω) is a required parameter for sine wave setter",
        "Frequency (ω) must be a number (containing only numbers 0-9 and decimal points "
        "('.').");
      CHECK_ENTRY_NUMBER(get_object_safe(data, "entB"),
        "Magnitude (M) is a required parameter for sine wave setter",
        "Magnitude (M) must be a number (containing only numbers 0-9 and decimal points "
        "('.').");
      CHECK_ENTRY_NUMBER(get_object_safe(data, "entC"),
        "Mean (A) is a required parameter for sine wave setter",
        "Mean (A) must be a number (containing only numbers 0-9 and decimal points "
        "('.').");
      break;

    case FORM_SETTER_ERROR:
      rv = 1;
      timestamp_error(data, 1, 0, "Unknown setpoint method. Chris has made a typo somewhere, please let him know ASAP so he can get this fixed!");
      break;
  }

  CHECK_ENTRY_NUMBER(get_object_safe(data, "entDI"),
      "Syringe diameter is a required field.",
      "Syringe diameter must be a number (containing only numbers 0-9 and decimal points "
      "('.').");

  CHECK_ENTRY_NUMBER(get_object_safe(data, "entBL"),
      "Stop buffer length is a required field.",
      "Buffer length must be a number (containing only numbers 0-9 and decimal points "
      "('.').");
  
  if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(get_object_safe(data, "radManual")))) {
    CHECK_ENTRY_NUMBER(get_object_safe(data, "entComposition"),
       "Suspension composition is a required field for composition control.",
       "Suspension composition must be a number (containing only numbers 0-9 and decimal points "
        "('.').");
    // TODO check that tuning file is set, too
  }
  else {
    switch (form_get_control_type(data)) {

      case FORM_CONTROL_NONE:
        break;

      case FORM_CONTROL_PID:
      case FORM_CONTROL_MEAS:
        CHECK_ENTRY_NUMBER(get_object_safe(data, "entKP"),
           "KP is a required field for PID control.",
           "KP must be a number (containing only numbers 0-9 and decimal points "
            "('.').");
        CHECK_ENTRY_NUMBER(get_object_safe(data, "entKI"),
           "KI is a required field for PID control.",
           "KI must be a number (containing only numbers 0-9 and decimal points "
            "('.').");
        CHECK_ENTRY_NUMBER(get_object_safe(data, "entKD"),
           "KD is a required field for PID control.",
           "KD must be a number (containing only numbers 0-9 and decimal points "
            "('.').");
        break;

      case FORM_CONTROL_ERROR:
        rv = 1;
        timestamp_error(data, 1, 0,
            "Unknown control method. Chris has made a typo somewhere, please let him know ASAP so he can fix this!");
        break;
    }
  }

  if (IS_EMPTY_ENTRY(get_object_safe(data, "entTag"))) {
    rv = 1;
    timestamp_error(data, 1, 0,
        "Tag should not be empty; use it to describe the run in a few words.");
  }

  // if tag contains spaces, periods, or underscores; they will be 
  // replaced with dashes
  // this is handled in the generate tag function.

  return rv;
}




// Set cursor to a specific type
void form_set_cursor(struct Data *data, const char *name)
{
  GdkWindow *win = gtk_widget_get_window(GTK_WIDGET(get_object_safe(data, "winMain")));

  if (win == NULL)
    return;

  GdkDisplay *display = gdk_display_get_default();
  GdkCursor *curs = gdk_cursor_new_from_name(display, name);

  gdk_window_set_cursor(GDK_WINDOW(win), curs);
}





// Depending on the situation, different parts of the form should be turned off or on
void form_set_sensitive(struct Data *data, int sensitivity_flag)
{
  // parts
  int 
    control = 0, 
    connection = 0,
    logging = 0, 
    connected = 0, 
    cursor_normal = 1,
    serial_available = 1;

  switch (sensitivity_flag){

  case FORM_ALL:
    control = 1;
    connection = 1;
    logging = 1;
    gtk_expander_set_expanded(GTK_EXPANDER(get_object_safe(data, "expControl")), 1);
    gtk_expander_set_expanded(GTK_EXPANDER(get_object_safe(data, "expSetpoint")), 1);
    gtk_expander_set_expanded(GTK_EXPANDER(get_object_safe(data, "expConnection")), 1);
    gtk_expander_set_expanded(GTK_EXPANDER(get_object_safe(data, "expSyringe")), 1);
    gtk_expander_set_expanded(GTK_EXPANDER(get_object_safe(data, "expLog")), 1);
    gtk_expander_set_expanded(GTK_EXPANDER(get_object_safe(data, "expStatus")), 1);
    break;
      
  case FORM_BUSY:
  case FORM_REFRESHING:
    cursor_normal = 0;
    serial_available = 0;
    break;

  case FORM_CONNECTED:
    connected = 1;
    gtk_expander_set_expanded(GTK_EXPANDER(get_object_safe(data, "expControl")), 0);
    gtk_expander_set_expanded(GTK_EXPANDER(get_object_safe(data, "expSetpoint")), 0);
    gtk_expander_set_expanded(GTK_EXPANDER(get_object_safe(data, "expConnection")), 0);
    gtk_expander_set_expanded(GTK_EXPANDER(get_object_safe(data, "expSyringe")), 0);
    gtk_expander_set_expanded(GTK_EXPANDER(get_object_safe(data, "expLog")), 0);
    gtk_expander_set_expanded(GTK_EXPANDER(get_object_safe(data, "expStatus")), 1);
    break;

  case FORM_DISCONNECTED:
    control = 1;
    connection = 1;
    logging = 1;
    gtk_expander_set_expanded(GTK_EXPANDER(get_object_safe(data, "expControl")), 1);
    gtk_expander_set_expanded(GTK_EXPANDER(get_object_safe(data, "expSetpoint")), 1);
    gtk_expander_set_expanded(GTK_EXPANDER(get_object_safe(data, "expConnection")), 0);
    gtk_expander_set_expanded(GTK_EXPANDER(get_object_safe(data, "expSyringe")), 1);
    gtk_expander_set_expanded(GTK_EXPANDER(get_object_safe(data, "expLog")), 1);
    gtk_expander_set_expanded(GTK_EXPANDER(get_object_safe(data, "expStatus")), 0);
    break;

  case FORM_NOSERIAL:
    connection = 1;
    serial_available = 0;
    gtk_expander_set_expanded(GTK_EXPANDER(get_object_safe(data, "expControl")), 0);
    gtk_expander_set_expanded(GTK_EXPANDER(get_object_safe(data, "expSetpoint")), 0);
    gtk_expander_set_expanded(GTK_EXPANDER(get_object_safe(data, "expConnection")), 1);
    gtk_expander_set_expanded(GTK_EXPANDER(get_object_safe(data, "expSyringe")), 0);
    gtk_expander_set_expanded(GTK_EXPANDER(get_object_safe(data, "expLog")), 0);
    gtk_expander_set_expanded(GTK_EXPANDER(get_object_safe(data, "expStatus")), 0);
    break;
      
  }

  // control
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "cmbControlMethod")), control);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "cmbSetpointFunction")), control);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "cmbControlledVariable")), control);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "entA")), control);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "entB")), control);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "entC")), control);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "entBL")), control);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "entDI")), control);

  // connection
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "cmbSerial")), connection);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "btnSerialRefresh")), connection);

  // logging
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "entTag")), logging);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "fchLogFolder")), logging);

  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "btnDisconnect")), connected);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "btnConnect")), (!connected) && serial_available );

  form_set_cursor(data, cursor_normal ? "normal" : "wait");
}




// safely retrieve gtkobject from GUI
GObject * get_object_safe(struct Data *data, const char *name)
{
  GObject *rv = gtk_builder_get_object(data->builder, name);

  if (rv == NULL) {
    timestamp_error(data, 1, 0, "GTK object not found in builder \"%s\"", name);
    timestamp_error(data, 1, 0, "This should only happen if Chris has been a numpty and mispelled something.", name);
    timestamp_error(data, 1, 0, "Let him know ASAP so he can fix it.", name);
    sleep(100);
    exit(1);
  }

  return rv;
}


#define CHECK_RAD_IS_CHECKED(N) gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(get_object_safe(data, N)))

// Get control option from the GUI
FORM_CONTROL_SELECTION form_get_control_type(struct Data *data)
{
  char *control_method_name = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(get_object_safe(data, "cmbControlMethod")));

  if (strcmp(control_method_name, "None") == 0) {
    return FORM_CONTROL_NONE;
  }
  else if (strcmp(control_method_name, "PID") == 0) {
    return FORM_CONTROL_PID;
  }
  else if (strcmp(control_method_name, "Passive") == 0) {
    return FORM_CONTROL_MEAS;
  }
  else {
    // should never get here
    return FORM_CONTROL_ERROR;
  }
}




// get setter option from GUI
FORM_SETTER_SELECTION form_get_setter_type(struct Data *data)
{

  char *setpoint_function_name = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(get_object_safe(data, "cmbSetpointFunction")));

  if (strcmp(setpoint_function_name, "Constant") == 0) {
    return FORM_SETTER_CONSTANT;
  }
  else if (strcmp(setpoint_function_name, "Linear ramp") == 0) {
    return FORM_SETTER_RAMP;
  }
  else if (strcmp(setpoint_function_name, "Step") == 0) {
    return FORM_SETTER_STEP;
  }
  else if (strcmp(setpoint_function_name, "Sine wave") == 0) {
    return FORM_SETTER_SINE;
  }
  else if (strcmp(setpoint_function_name, "Square wave") == 0) {
    return FORM_SETTER_SQUARE;
  }
  else {
    return FORM_SETTER_ERROR;
  }

}




// get controlled variable from GUI
FORM_CONTROLLED_VAR form_get_controlled_var(struct Data *data)
{
  char *controlled_variable_name = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(get_object_safe(data, "cmbControlledVariable")));

  if (strcmp(controlled_variable_name, "Flowrate") == 0) {
    return FORM_VAR_FLOW;
  }
  else if (strcmp(controlled_variable_name, "Force") == 0) {
    return FORM_VAR_FORCE;
  }
  else {
    return FORM_VAR_ERROR;
  }

}




// Called when a setpoint/controller/variable combobox is changed
void form_setter_update(struct Data *data) 
{
  GObject 
    *lblA = get_object_safe(data, "lblA"), 
    *lblB = get_object_safe(data, "lblB"), 
    *lblC = get_object_safe(data, "lblC"),
    *entA = get_object_safe(data, "entA"),
    *entB = get_object_safe(data, "entB"),
    *entC = get_object_safe(data, "entC"),
    *lblTitle = get_object_safe(data, "lblSetterTitle"),
    *lblDesc = get_object_safe(data, "lblSetterDesc"),
    *lblFormula = get_object_safe(data, "lblSetterFormula");

  int controlled = form_get_control_type(data) != FORM_CONTROL_NONE; // what about FORM_CONTROL_ERROR?
  int flow_control = form_get_controlled_var(data) == FORM_VAR_FLOW;

  //gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "btnTuning")), controlled);
  //gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "radFlowControl")), controlled);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "cmbControlledVariable")), controlled);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "expTuning")), controlled);

  if (!controlled)
    gtk_expander_set_expanded(GTK_EXPANDER(get_object_safe(data, "expTuning")), 0);


  switch (form_get_setter_type(data)) {

    case FORM_SETTER_CONSTANT:
      gtk_label_set_text(GTK_LABEL(lblTitle), "Constant setter");
      gtk_label_set_text(GTK_LABEL(lblDesc), "Constant value for setpoint.");
      gtk_label_set_markup(GTK_LABEL(lblFormula), !controlled ? "DC = <b>C</b>" : (flow_control ? "Q<sub>SP</sub> = <b>C</b>" : "F<sub>SP</sub> = <b>C</b>")   );

      gtk_label_set_markup(GTK_LABEL(lblA), !controlled ? "<b>C</b> (8 bit):" : (flow_control ? "<b>C</b> (ml/s):" : "<b>C</b> (N):"));
      gtk_label_set_markup(GTK_LABEL(lblB), "<i>n/a</i>:");
      gtk_label_set_markup(GTK_LABEL(lblC), "<i>n/a</i>:");

      gtk_widget_set_sensitive(GTK_WIDGET(entA), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(entB), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(entC), 0);
      break;

    case FORM_SETTER_RAMP:
      gtk_label_set_text(GTK_LABEL(lblTitle), "Ramp setter");
      gtk_label_set_text(GTK_LABEL(lblDesc), "Setpoint increases linearly with time.");
      gtk_label_set_markup(GTK_LABEL(lblFormula), !controlled ? "DC = <b>M</b> t + <b>C</b>" : (flow_control ? "Q<sub>SP</sub> = <b>M</b> t + <b>C</b>" : "F<sub>SP</sub> = <b>M</b> t + <b>C</b>"));

      gtk_label_set_markup(GTK_LABEL(lblA), !controlled ? "<b>M</b> (8 bit/s):" : (flow_control ? "<b>M</b> (ml/s<sup>2</sup>):" : "<b>M</b> (N/s):"));
      gtk_label_set_markup(GTK_LABEL(lblB), !controlled ? "<b>C</b> (8 bit):" : (flow_control ? "<b>C</b> (ml/s):" : "<b>C</b> (N):"));
      gtk_label_set_markup(GTK_LABEL(lblC), "<i>n/a</i>:");

      gtk_widget_set_sensitive(GTK_WIDGET(entA), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(entB), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(entC), 0);
      break;

    case FORM_SETTER_STEP:
      gtk_label_set_text(GTK_LABEL(lblTitle), "Step setter");
      gtk_label_set_text(GTK_LABEL(lblDesc), "Constant setpoint until a time when jumps up to another constant. Useful for process modelling.");
      gtk_label_set_markup(GTK_LABEL(lblFormula), 
          !controlled ? "DC = <b>I</b> if t &lt; t<sub>c</sub> otherwise <b>F</b>" : 
          (flow_control ? "Q<sub>SP</sub> = <b>I</b> if t &lt; t<sub>c</sub> otherwise <b>F</b>" : 
           "F<sub>SP</sub> = <b>I</b> if t &lt; t<sub>c</sub> otherwise <b>F</b>"));

      gtk_label_set_markup(GTK_LABEL(lblA), 
          !controlled ? "<b>I</b> (8 bit):" : 
          (flow_control ? "<b>I</b> (ml/s):" : 
           "<b>I</b> (N):"));
      gtk_label_set_markup(GTK_LABEL(lblB), "<b>t<sub>c</sub></b> (s):");
      gtk_label_set_markup(GTK_LABEL(lblC), 
          !controlled ? "<b>F</b> (8 bit):" : 
          (flow_control ? "<b>F</b> (ml/s):" : 
           "<b>F</b> (N):"));

      gtk_widget_set_sensitive(GTK_WIDGET(entA), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(entB), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(entC), 1);
      break;

    case FORM_SETTER_SINE:
      gtk_label_set_text(GTK_LABEL(lblTitle), "Sine wave setter");
      gtk_label_set_text(GTK_LABEL(lblDesc), "Setpoint is an offset sine wave.");
      gtk_label_set_markup(GTK_LABEL(lblFormula), !controlled ? "DC = <b>M</b> sin(π <b>ω</b> t) + <b>A</b>" : 
          (flow_control ? "Q<sub>SP</sub> = <b>M</b> sin(π <b>ω</b> t) + <b>A</b>" : 
           "F<sub>SP</sub> = <b>M</b> sin(π <b>ω</b> t) + <b>A</b>"));

      gtk_label_set_markup(GTK_LABEL(lblA), "<b>ω</b> (Hz):");
      gtk_label_set_markup(GTK_LABEL(lblB), !controlled ? "<i>M</i> (8 bit):" : (flow_control ? "<i>M</i> (ml/s):" : "<i>M</i> (N):"));
      gtk_label_set_markup(GTK_LABEL(lblC), !controlled ? "<i>A</i> (8 bit):" : (flow_control ? "<i>A</i> (ml/s):" : "<i>A</i> (N):"));

      gtk_widget_set_sensitive(GTK_WIDGET(entA), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(entB), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(entC), 1);
      break;

    case FORM_SETTER_SQUARE:
      gtk_label_set_text(GTK_LABEL(lblTitle), "Square wave setter");
      gtk_label_set_text(GTK_LABEL(lblDesc), "Setpoint is a square wave.");
      gtk_label_set_markup(GTK_LABEL(lblFormula), !controlled ? "DC = if (round(<b>ω</b> t) % 2 == 1) then <b>A</b> else <b>B</b>" : 
          (flow_control ? "Q<sub>SP</sub> = if (round(<b>ω</b> t) % 2 == 1) then <b>A</b> else <b>B</b>" : 
           "F<sub>SP</sub> = if (round(<b>ω</b> t) % 2 == 1) then <b>A</b> else <b>B</b>"));

      gtk_label_set_markup(GTK_LABEL(lblA), "<b>ω</b> (Hz):");
      gtk_label_set_markup(GTK_LABEL(lblB), !controlled ? "<i>A</i> (8 bit):" : (flow_control ? "<i>M</i> (ml/s):" : "<i>M</i> (N):"));
      gtk_label_set_markup(GTK_LABEL(lblC), !controlled ? "<i>B</i> (8 bit):" : (flow_control ? "<i>A</i> (ml/s):" : "<i>A</i> (N):"));

      gtk_widget_set_sensitive(GTK_WIDGET(entA), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(entB), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(entC), 1);
      break;

    case FORM_SETTER_ERROR:

      gtk_label_set_text(GTK_LABEL(lblTitle), "Unknown setpoint function");
      gtk_label_set_text(GTK_LABEL(lblDesc), "Chris has forgotten to account for one of the setpoint functions or has made a typo. Please let him know ASAP so this can be fixed!");
      gtk_label_set_markup(GTK_LABEL(lblFormula), "<i>n/a</i>");

      gtk_label_set_markup(GTK_LABEL(lblA), "<i>n/a</i>:");
      gtk_label_set_markup(GTK_LABEL(lblB), "<i>n/a</i>:");
      gtk_label_set_markup(GTK_LABEL(lblC), "<i>n/a</i>:");

      gtk_widget_set_sensitive(GTK_WIDGET(entA), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(entB), 0);
      gtk_widget_set_sensitive(GTK_WIDGET(entC), 0);
      break;

  }
}




// get setter params, formatted into string.
char *form_get_setter_params(struct Data *data, int n)
{
  char *rv = NULL;

  if (n > 0) {
    rv = calloc(PACKET_VALUE_LEN+1, sizeof(char));
    GObject *ent_A = get_object_safe(data, "entA");
    strncat(rv, gtk_entry_get_text(GTK_ENTRY(ent_A)), PACKET_VALUE_LEN-1);
  }

  if (n > 1) {
    GObject *ent_B = get_object_safe(data, "entB");
    strncat(rv, gtk_entry_get_text(GTK_ENTRY(ent_B)), PACKET_VALUE_LEN-1);
  }

  if (n > 2) {
    GObject *ent_C = get_object_safe(data, "entC");
    strncat(rv, gtk_entry_get_text(GTK_ENTRY(ent_C)), PACKET_VALUE_LEN-1);
  }

  return rv;
}














char *form_get_pid_tuning(struct Data *data)
{
  int manual = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(get_object_safe(data, "radManual")));
  char *rv = calloc(PACKET_VALUE_LEN+1, sizeof(char));

  if (manual) {
    GObject *ent_KP = get_object_safe(data, "entKP");
    GObject *ent_KI = get_object_safe(data, "entKI");
    GObject *ent_KD = get_object_safe(data, "entKD");
    snprintf(rv, PACKET_VALUE_LEN, "%s,%s,%s", gtk_entry_get_text(GTK_ENTRY(ent_KP)), gtk_entry_get_text(GTK_ENTRY(ent_KI)), gtk_entry_get_text(GTK_ENTRY(ent_KD)));
  }
  else {
    GObject *composition_entry = get_object_safe(data, "entComposition");
    double composition = atof(gtk_entry_get_text(GTK_ENTRY(composition_entry)));
    unsigned int n = data->composition_data->n;
    double kp = interp(data->composition_data->cm, data->composition_data->kp, n, composition);
    double ki = interp(data->composition_data->cm, data->composition_data->ki, n, composition);
    double kd = interp(data->composition_data->cm, data->composition_data->kd, n, composition);
    snprintf(rv, PACKET_VALUE_LEN, "%f,%f,%f", kp, ki, kd);
  }
  return rv;
}




// get bufflen, dia from GUI
char *form_get_bldi_packet(struct Data *data)
{
  GObject *ent_bl = get_object_safe(data, "entBL");
  GObject *ent_di = get_object_safe(data, "entDI");
  char *rv = calloc(PACKET_VALUE_LEN+1, sizeof(char));
  snprintf(rv, PACKET_VALUE_LEN, "%s,%s", gtk_entry_get_text(GTK_ENTRY(ent_bl)), gtk_entry_get_text(GTK_ENTRY(ent_di)));
  return rv;
}


char form_get_controlled_var_char(struct Data *data)
{
  // get character representing controlled var
  switch (form_get_control_type(data)) {

    case FORM_CONTROL_ERROR:
    case FORM_CONTROL_NONE:
      return 'D';

    case FORM_CONTROL_PID:
    case FORM_CONTROL_MEAS:
      switch (form_get_controlled_var(data)) {

        case FORM_VAR_FLOW:
          return 'Q';

        case FORM_VAR_FORCE:
          return 'F';

        case FORM_VAR_ERROR:
          return 'D';
      }

  }

  // control will not get here, but let's appease the linter
  return 'D';
}

char form_get_controller_type_char(struct Data *data)
{
  switch(form_get_control_type(data)) {

    case FORM_CONTROL_PID:
      return 'P';

    case FORM_CONTROL_MEAS:
      return 'M';

    case FORM_CONTROL_NONE:
    case FORM_CONTROL_ERROR:
      return 'E';
  }

  // control will not get here, but let's appease the linter
  return 'E';
}


char *form_get_setter_packet(struct Data *data)
{
  char *setpoint = calloc(PACKET_VALUE_LEN+1, sizeof(char)), 
       setter_ch = 0, 
       *setter_params = NULL;

  switch (form_get_setter_type(data)) {

    case FORM_SETTER_CONSTANT:
      setter_ch = 'C';
      setter_params = form_get_setter_params(data, 1);
      break;

    case FORM_SETTER_RAMP:
      setter_ch = 'R';
      setter_params = form_get_setter_params(data, 2);
      break;

    case FORM_SETTER_STEP:
    case FORM_SETTER_SINE:
    case FORM_SETTER_SQUARE:
      setter_ch = 'S';
      setter_params = form_get_setter_params(data, 3);
      break;

    case FORM_SETTER_ERROR:
      setter_ch = 'E';
      setter_params = "0,0,0";
      break;

  }

  snprintf(setpoint, PACKET_VALUE_LEN, "%c%c%s", form_get_controlled_var_char(data), setter_ch, setter_params);
  return setpoint;
}

char *form_get_controller_packet(struct Data *data)
{
  char controller_type_ch = form_get_controller_type_char(data);
  char *tuning = form_get_pid_tuning(data);
  char *rv = calloc(PACKET_VALUE_LEN+1, sizeof(char));
  snprintf(rv, PACKET_VALUE_LEN, "%c%s", controller_type_ch, tuning);
  free(tuning);
  return rv;
}
