#ifndef UNITCONVERSIONS_H_
#define UNITCONVERSIONS_H_

/// Unit conversion factors at double precision. Pressure is carried in kgf/cm^2
/// and temperature in degrees Celsius; the correlations ask for SI or field units.
namespace units {

/// The standard acceleration of gravity, exact by definition (3rd CGPM, 1901). The kilogram-force is a
/// kilogram under it, which makes the kgf/cm^2 9.80665e4 Pa.
inline constexpr double kStandardGravity = 9.80665;
inline constexpr double kPascalPerKgfPerCm2 = 98066.5;
inline constexpr double kPascalPerAtmosphere = 101325.;
inline constexpr double kPascalPerBar = 100000.;
/// A pound-force, 0.45359237 kg under 9.80665 m/s^2, over a square inch, (0.0254 m)^2.
inline constexpr double kPascalPerPsi = 6894.75729316836134;
inline constexpr double kPsiPerKgfPerCm2 = kPascalPerKgfPerCm2 / kPascalPerPsi;
inline constexpr double kAtmosphereInKgfPerCm2 = kPascalPerAtmosphere / kPascalPerKgfPerCm2;
inline constexpr double kAtmosphereInPsi = kPascalPerAtmosphere / kPascalPerPsi;
inline constexpr double kKgfPerCm2PerBar = kPascalPerBar / kPascalPerKgfPerCm2;
inline constexpr double kBarPerKgfPerCm2 = kPascalPerKgfPerCm2 / kPascalPerBar;

inline constexpr double kZeroCelsiusInKelvin = 273.15;
inline constexpr double kZeroFahrenheitInRankine = 459.67;

inline constexpr double celsiusToFahrenheit(double celsius) {
    return 1.8 * celsius + 32;
}

/// The oil barrel, 42 US gallons of 231 cubic inches, and the cubic foot, 0.3048^3 m^3.
inline constexpr double kCubicMetrePerBarrel = 0.158987294928;
inline constexpr double kCubicMetrePerCubicFoot = 0.028316846592;
inline constexpr double kBarrelPerCubicMetre = 1. / kCubicMetrePerBarrel;
inline constexpr double kCubicFootPerCubicMetre = 1. / kCubicMetrePerCubicFoot;

/// The centimetre per metre, and per inch: 2.54 cm by definition.
inline constexpr double kCentimetrePerMetre = 100.;
inline constexpr double kCentimetrePerInch = 2.54;

inline constexpr double kSecondsPerDay = 86400.;
inline constexpr double kPascalSecondPerCentipoise = 1.e-3;

static_assert(kStandardGravity == 9.80665);
static_assert(kPascalPerKgfPerCm2 == 98066.5);
static_assert(kStandardGravity * 1.e4 == kPascalPerKgfPerCm2);
static_assert(kPascalPerAtmosphere == 101325.);
static_assert(kPascalPerBar == 100000.);
static_assert(kPascalPerPsi == 6894.757293168362);
static_assert(kPsiPerKgfPerCm2 == 14.223343307119562);
static_assert(kAtmosphereInKgfPerCm2 == 1.0332274527998857);
static_assert(kAtmosphereInPsi == 14.695948775513449);
static_assert(kKgfPerCm2PerBar == 1.0197162129779282);
static_assert(kBarPerKgfPerCm2 == 0.980665);
static_assert(kZeroCelsiusInKelvin == 273.15);
static_assert(kZeroFahrenheitInRankine == 459.67);
static_assert(kCubicMetrePerBarrel == 0.158987294928);
static_assert(kCubicMetrePerCubicFoot == 0.028316846592);
static_assert(kBarrelPerCubicMetre == 6.289810770432105);
static_assert(kCubicFootPerCubicMetre == 35.31466672148859);
static_assert(kCentimetrePerMetre == 100.);
static_assert(kCentimetrePerInch == 2.54);
static_assert(kSecondsPerDay == 86400.);
static_assert(kPascalSecondPerCentipoise == 1.e-3);
static_assert(celsiusToFahrenheit(0.) == 32. && celsiusToFahrenheit(100.) == 212.);

}  // namespace units

#endif  // UNITCONVERSIONS_H_
