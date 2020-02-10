#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>


#include "form.h"
#include "error.h"
#include "util.h"


#define IS_EMPTY_ENTRY(INP) (strlen(gtk_entry_get_text(GTK_ENTRY(INP))) == 0)
#define CHECK_ENTRY_NUMBER(DATA,MESG_EMPTY,MESG_NOT_NUM) \
  if (IS_EMPTY_ENTRY(DATA)) { \
      rv = 1; \
      timestamp_error(data, 1, MESG_EMPTY); \
    } \
    else { \
      if (is_not_number( gtk_entry_get_text(GTK_ENTRY(DATA)) )) { \
        rv = 1; \
        timestamp_error(data, 1, MESG_NOT_NUM); \
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
  }

  if (IS_EMPTY_ENTRY(get_object_safe(data, "entTag"))) {
    rv = 1;
    timestamp_error(data, 1,
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
    cursor_normal = 1;

  switch (sensitivity_flag){

  case FORM_ALL:
    control = 1;
    connection = 1;
    logging = 1;
    break;
      
  case FORM_BUSY:
  case FORM_REFRESHING:
    cursor_normal = 0;
    break;

  case FORM_CONNECTED:
    connected = 1;
    break;

  case FORM_DISCONNECTED:
    control = 1;
    connection = 1;
    logging = 1;
    break;

  case FORM_NOSERIAL:
    connection = 1;
    break;
      
  }

  // control
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "radPID")), control);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "radNoControl")), control);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "radPassive")), control);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "btnTuning")), control);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "radFlowControl")), control);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "radForceControl")), control);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "radConst")), control);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "radRamp")), control);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "radStep")), control);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "radSine")), control);
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
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "chkTime")), logging);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "chkForce")), logging);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "chkFlowrate")), logging);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "chkCA")), logging);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "chkLoadcell")), logging);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "chkTicks")), logging);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "fchLogFolder")), logging);

  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "btnDisconnect")), connected);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "btnConnect")), !connected);

  form_set_cursor(data, cursor_normal ? "normal" : "wait");
}




// safely retrieve gtkobject from GUI
GObject * get_object_safe(struct Data *data, const char *name)
{
  GObject *rv = gtk_builder_get_object(data->builder, name);

  if (rv == NULL) {
    timestamp_error(data, 1, "GTK object not found in builder \"%s\"", name);
    timestamp_error(data, 1, "This should only happen if Chris has been a numpty and mispelled something.", name);
    timestamp_error(data, 1, "Let him know ASAP so he can fix it.", name);
    sleep(100);
    exit(1);
  }

  return rv;
}


#define CHECK_RAD_IS_CHECKED(N) gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(get_object_safe(data, N)))

// Get control option from the GUI
FORM_CONTROL_SELECTION form_get_control_type(struct Data *data)
{
  if (CHECK_RAD_IS_CHECKED("radPID")) {
    return FORM_CONTROL_PID;
  }
  else if (CHECK_RAD_IS_CHECKED("radPassive")) {
    return FORM_CONTROL_MEAS;
  }
  else {
    return FORM_CONTROL_NONE;
  }
}




// get setter option from GUI
FORM_SETTER_SELECTION form_get_setter_type(struct Data *data)
{

  if (CHECK_RAD_IS_CHECKED("radConst")) {
    return FORM_SETTER_CONSTANT;
  }
  else if (CHECK_RAD_IS_CHECKED("radRamp")) {
    return FORM_SETTER_RAMP;
  }
  else if (CHECK_RAD_IS_CHECKED("radStep")) {
    return FORM_SETTER_STEP;
  }
  else {
    return FORM_SETTER_SINE;
  }
}




// get controlled variable from GUI
FORM_CONTROLLED_VAR form_get_controlled_var(struct Data *data)
{
  if (CHECK_RAD_IS_CHECKED("radFlowControl")) {
    return FORM_VAR_FLOW;
  }
  else {
    return FORM_VAR_FORCE;
  }
  // or none?
}




// Called when a setpoint radio button is toggled
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

  int controlled = form_get_control_type(data) != FORM_CONTROL_NONE;
  int flow_control = form_get_controlled_var(data) == FORM_VAR_FLOW;

  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "btnTuning")), controlled);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "radFlowControl")), controlled);
  gtk_widget_set_sensitive(GTK_WIDGET(get_object_safe(data, "radForceControl")), controlled);

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
      gtk_label_set_markup(GTK_LABEL(lblFormula), !controlled ? "DC = s<b>M</b> in(π <b>ω</b> t) + <b>A</b>" : (flow_control ? "Q<sub>SP</sub> = <b>M</b> sine(π <b>ω</b> t) + <b>A</b>" : "F<sub>SP</sub> = <b>M</b> sin(π <b>ω</b> t) + <b>A</b>"));

      gtk_label_set_markup(GTK_LABEL(lblA), "<b>ω</b> (Hz):");
      gtk_label_set_markup(GTK_LABEL(lblB), !controlled ? "<i>M</i> (8 bit/s):" : (flow_control ? "<i>M</i> (ml/s):" : "<i>M</i> (N):"));
      gtk_label_set_markup(GTK_LABEL(lblC), !controlled ? "<i>A</i> (8 bit/s):" : (flow_control ? "<i>A</i> (ml/s):" : "<i>A</i> (N):"));

      gtk_widget_set_sensitive(GTK_WIDGET(entA), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(entB), 1);
      gtk_widget_set_sensitive(GTK_WIDGET(entC), 1);
      break;

  }
}




// get what logging options are set
int form_get_log_options(struct Data *data)
{

  GObject 
    *log_time_chk = get_object_safe(data, "chkTime"),
    *log_force_chk = get_object_safe(data, "chkForce"),
    *log_flow_chk = get_object_safe(data, "chkFlowrate"),
    *log_ca_chk = get_object_safe(data, "chkCA"),
    *log_loadcell_chk = get_object_safe(data, "chkLoadcell"),
    *log_ticks_chk = get_object_safe(data, "chkTicks");

  int log_time = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(log_time_chk)), 
      log_force = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(log_force_chk)), 
      log_flow = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(log_flow_chk)), 
      log_ca = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(log_ca_chk)), 
      log_loadcell = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(log_loadcell_chk)), 
      log_ticks = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(log_ticks_chk));

  return (log_time << 5) + (log_force << 4) + (log_flow << 3) + (log_ca << 2) + (log_loadcell << 1) + log_ticks;
}




// get setter params, formatted into string.
char *form_get_const_setter_params(struct Data *data)
{
  GObject *ent_A = get_object_safe(data, "entA");
  return (char *)gtk_entry_get_text(GTK_ENTRY(ent_A));
}




// get setter params, formatted into string.
char *form_get_ramp_setter_params(struct Data *data)
{
  GObject *ent_A = get_object_safe(data, "entA");
  GObject *ent_B = get_object_safe(data, "entB");
  char *rv = malloc(70*sizeof(char));
  sprintf(rv, "%s,%s", gtk_entry_get_text(GTK_ENTRY(ent_A)), gtk_entry_get_text(GTK_ENTRY(ent_B)));
  return rv;
}




// get setter params, formatted into string.
char *form_get_step_setter_params(struct Data *data)
{
  GObject *ent_A = get_object_safe(data, "entA");
  GObject *ent_B = get_object_safe(data, "entB");
  GObject *ent_C = get_object_safe(data, "entC");
  char *rv = malloc(70*sizeof(char));
  sprintf(rv, "%s,%s,%s", gtk_entry_get_text(GTK_ENTRY(ent_A)), gtk_entry_get_text(GTK_ENTRY(ent_B)), gtk_entry_get_text(GTK_ENTRY(ent_C)));
  return rv;
}




// get setter params, formatted into string.
char *form_get_sine_setter_params(struct Data *data)
{
  GObject *ent_A = get_object_safe(data, "entA");
  GObject *ent_B = get_object_safe(data, "entB");
  GObject *ent_C = get_object_safe(data, "entC");
  char *rv = malloc(70*sizeof(char));
  sprintf(rv, "%s,%s,%s", gtk_entry_get_text(GTK_ENTRY(ent_A)), gtk_entry_get_text(GTK_ENTRY(ent_B)), gtk_entry_get_text(GTK_ENTRY(ent_C)));
  return rv;
}

char *_get_pid_tuning(struct Data *data)
{
  int manual = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(get_object_safe(data, "radManual")));
  char *rv = malloc(70*sizeof(char));

  if (manual) {
    GObject *ent_KP = get_object_safe(data, "entKP");
    GObject *ent_KI = get_object_safe(data, "entKI");
    GObject *ent_KD = get_object_safe(data, "entKD");
    sprintf(rv, "%s,%s,%s", gtk_entry_get_text(GTK_ENTRY(ent_KP)), gtk_entry_get_text(GTK_ENTRY(ent_KI)), gtk_entry_get_text(GTK_ENTRY(ent_KD)));
  }
  else {
    GObject *composition_entry = get_object_safe(data, "entComposition");
    double composition = atof(gtk_entry_get_text(GTK_ENTRY(composition_entry)));
    unsigned int n = data->composition_data->n;
    double kp = interp(data->composition_data->cm, data->composition_data->kp, n, composition);
    double ki = interp(data->composition_data->cm, data->composition_data->ki, n, composition);
    double kd = interp(data->composition_data->cm, data->composition_data->kd, n, composition);
    sprintf(rv, "%f,%f,%f", kp, ki, kd);
  }
  return rv;
}




// get PID tuning params from dialog
char *form_get_pid_params(struct Data *data)
{
  char *tuning = _get_pid_tuning(data);
  char *rv = malloc(70*sizeof(char));
  sprintf(rv, "P%s", tuning);
  free(tuning);
  return rv;
}




// get MEAS tuning params from dialog
char *form_get_meas_params(struct Data *data)
{
  char *tuning = _get_pid_tuning(data);
  char *rv = malloc(70*sizeof(char));
  sprintf(rv, "M%s", tuning);
  free(tuning);
  return rv;
}




// get bufflen, dia from GUI
char *form_get_bldi_data(struct Data *data)
{
  GObject *ent_bl = get_object_safe(data, "entBL");
  GObject *ent_di = get_object_safe(data, "entDI");
  char *rv = malloc(70*sizeof(char));
  sprintf(rv, "%s,%s", gtk_entry_get_text(GTK_ENTRY(ent_bl)), gtk_entry_get_text(GTK_ENTRY(ent_di)));
  return rv;
}
