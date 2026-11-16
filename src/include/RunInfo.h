#ifndef RUNINFO_H_
#define RUNINFO_H_

#include <ctime>
#include <string>

/// Application or simulator version string.
extern std::string versao;
/// Global simulation start timestamp.
extern std::time_t nowGlobIni;
/// Broken-down local time corresponding to nowGlobIni.
extern std::tm *ltmGlobIni;
/// Day component of the simulation start time.
extern int diaIni;
/// Hour component of the simulation start time.
extern int horaIni;
/// Minute component of the simulation start time.
extern int minutoIni;
/// Second component of the simulation start time.
extern int segundoIni;
/// Global simulation end timestamp.
extern std::time_t nowGlobFim;
/// Broken-down local time corresponding to nowGlobFim.
extern std::tm *ltmGlobFim;

#endif /* RUNINFO_H_ */
