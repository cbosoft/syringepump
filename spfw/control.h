#define SPEED_HIST_LEN 32
#define ERR_HIST_LEN 100

double getControlAction(double previous_control_action, double speed, double force);
void controlInit();
