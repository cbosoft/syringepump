// -*- mode: c++ -*-
#define SPEED_HIST_LEN 32
#define ERR_HIST_LEN 100
#define MAX_DC 255.0
#define MIN_DC 0.0

enum CONTROLLER_TYPE {CONTROL_PID, CONTROL_MEAS, CONTROL_NONE};
enum CONTROLLED_VARIABLE {CONTROLLED_FLOWRATE, CONTROLLED_FORCE};


class Controller {
private:
  int controlled_variable;
  
  double pid_kp;
  double pid_ki;
  double pid_kd;
  double err1 = 0.0, err2 = 0.0;
  
  double ca = -1.0;
  double previous_ca = 0.0;

  unsigned long measure_time;
  double previous_input = 0.0;
public:
  Controller() { this->ca = 0.0; this->previous_ca = 0.0; };

  virtual double get_action(double setpoint, double flowrate, double force);
  void set_controlled_variable(int controlled_variable) {this->controlled_variable = controlled_variable;}

  friend class PIDController;
  friend class NoController;
  friend class MeasureController;
};


class PIDController : public Controller {
public:
  PIDController(double kp, double ki, double kd) {
    this->pid_kp = kp;
    this->pid_ki = ki;
    this->pid_kd = kd;
    this->ca = 0.0;
    this->previous_ca = 0.0;
  }
  double get_action(double setpoint, double flowrate, double force);
};


class NoController : public Controller {
public:
  NoController() {};
  double get_action(double setpoint, double flowrate, double force) { return setpoint; };
};


class MeasureController : public Controller {
  private:
    int passive;
  public:
    MeasureController(unsigned long measure_time) {
      this->measure_time = measure_time;
      this->passive = 0;
    }
    double get_action(double setpoint, double flowrate, double force);
};




Controller *controlInit();
