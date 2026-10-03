#pragma once

#include "esphome.h"
#include <cmath>

namespace klok {

// ============================================================================
// INSTELLINGEN
// ============================================================================

// Tijd altijd tonen.
static const bool SHOW_TIME = true;

// Weekdag op R6 tonen.
static const bool SHOW_WEEKDAY = true;

// Datum op R2 tonen.
static const bool SHOW_DATE = true;

// Temperatuur op R11 tonen zodra een geldige temperatuur beschikbaar is.
static const bool SHOW_TEMPERATURE = true;

// Knipperende dubbele punt?
// false = continu aan
// true  = iedere seconde aan/uit
static const bool BLINK_COLON = false;

// RGB-kleur van alle segmenten.
static const esphome::Color COLOR(255, 255, 255);

// ============================================================================
// 7-segment cijfers
//
// bit 0 = a
// bit 1 = b
// bit 2 = c
// bit 3 = d
// bit 4 = e
// bit 5 = f
// bit 6 = g
//
//       aaa
//      f   b
//      f   b
//       ggg
//      e   c
//      e   c
//       ddd
// ============================================================================

static const uint8_t DIGIT[10] = {
    0x3F,  // 0 = abcdef
    0x06,  // 1 = bc
    0x5B,  // 2 = abdeg
    0x4F,  // 3 = abcdg
    0x66,  // 4 = bcfg
    0x6D,  // 5 = acdfg
    0x7D,  // 6 = acdefg
    0x07,  // 7 = abc
    0x7F,  // 8 = abcdefg
    0x6F   // 9 = abcdfg
};


// ============================================================================
// GENERIEKE FUNCTIES
// ============================================================================

// Zet een volledige strip uit.
inline void clear_strip(esphome::light::AddressableLight &strip) {
  for (int i = 0; i < strip.size(); i++) {
    strip[i] = esphome::Color(0, 0, 0);
  }
}


// Teken een 7-segment cijfer wanneer ieder segment uit twee
// opeenvolgende LEDs bestaat.
//
// Bijvoorbeeld:
//   {0,2,4,8,10,12,6}
inline void draw_2led_start(
    esphome::light::AddressableLight &strip,
    const uint8_t starts[7],
    uint8_t mask,
    const esphome::Color &color
) {
  for (int segment = 0; segment < 7; segment++) {
    if (mask & (1 << segment)) {
      strip[starts[segment]] = color;
      strip[starts[segment] + 1] = color;
    }
  }
}


// Teken een 7-segment cijfer wanneer ieder segment expliciet
// uit twee willekeurige LEDs bestaat.
inline void draw_2led_pairs(
    esphome::light::AddressableLight &strip,
    const uint8_t pairs[7][2],
    uint8_t mask,
    const esphome::Color &color
) {
  for (int segment = 0; segment < 7; segment++) {
    if (mask & (1 << segment)) {
      strip[pairs[segment][0]] = color;
      strip[pairs[segment][1]] = color;
    }
  }
}


// Teken een 7-segment cijfer met één LED per segment.
inline void draw_1led_segments(
    esphome::light::AddressableLight &strip,
    const uint8_t segments[7],
    uint8_t mask,
    const esphome::Color &color
) {
  for (int segment = 0; segment < 7; segment++) {
    if (mask & (1 << segment)) {
      strip[segments[segment]] = color;
    }
  }
}


// ============================================================================
// R6 - 36 LEDs
//
// R6:
//
//   0-13   = tweede uurcijfer (elk segment 2 LEDs)
//   14-28  = eerste uurcijfer
//   22     = PM
//   29-35  = weekdag
// ============================================================================


// -----------------------------------------------------------------------------
// Uur-eenheden
//
// BEVESTIGD
// -----------------------------------------------------------------------------

static const uint8_t R6_HOUR_ONES[7] = {
    0,   // a
    2,   // b
    4,   // c
    8,   // d
    10,  // e
    12,  // f
    6    // g
};


// -----------------------------------------------------------------------------
// Uur-tientallen
//
// VOLLEDIG GEMETEN
// -----------------------------------------------------------------------------

static const uint8_t R6_HOUR_TENS[7][2] = {
    {23, 24},  // a
    {14, 15},  // b
    {16, 17},  // c
    {18, 19},  // d
    {25, 26},  // e
    {27, 28},  // f
    {20, 21}   // g
};


// PM-indicator.
static const uint8_t R6_PM = 22;


// -----------------------------------------------------------------------------
// Weekdag
//
// ESPHome:
//   1 = zondag
//   2 = maandag
//   3 = dinsdag
//   4 = woensdag
//   5 = donderdag
//   6 = vrijdag
//   7 = zaterdag
//
// GEMETEN
// -----------------------------------------------------------------------------

static const uint8_t R6_WEEKDAY[7] = {
    29,  // Sunday
    35,  // Monday
    34,  // Tuesday
    33,  // Wednesday
    32,  // Thursday
    31,  // Friday
    30   // Saturday
};


// ============================================================================
// R1 - 32 LEDs
//
// R1 = minuten + dubbele punt + alarm indicators
// ============================================================================


// -----------------------------------------------------------------------------
// Eerste minuutcijfer: tientallen
//
// HH:Mx
// -----------------------------------------------------------------------------

static const uint8_t R1_MIN_TENS[7][2] = {
    {3,  4},   // a
    {5,  6},   // b
    {23, 24},  // c
    {27, 28},  // d
    {29, 30},  // e
    {1,  2},   // f
    {25, 26}   // g
};


// -----------------------------------------------------------------------------
// Tweede minuutcijfer: eenheden
//
// HH:xM
// -----------------------------------------------------------------------------

static const uint8_t R1_MIN_ONES[7][2] = {
    {9,  10},  // a
    {11, 12},  // b
    {15, 16},  // c
    {17, 18},  // d
    {21, 22},  // e
    {7,  8},   // f
    {19, 20}   // g
};


// Dubbele punt.
static const uint8_t R1_COLON_TOP = 0;
static const uint8_t R1_COLON_BOTTOM = 31;


// Alarm indicators.
static const uint8_t R1_ALARM2 = 13;
static const uint8_t R1_ALARM1 = 14;


// ============================================================================
// R2 - 25 LEDs
//
// R2 = datum
//
//   LED 0     = D
//   LED 1-7   = dag-eenheden
//   LED 8-14  = dag-tientallen
//   LED 15    = M
//   LED 16-22 = maand-eenheid
//   LED 23-24 = maand-tiental "1"
// ============================================================================


// -----------------------------------------------------------------------------
// Dag-eenheden
//
// DD
// -----------------------------------------------------------------------------

static const uint8_t R2_DAY_ONES[7] = {
    4,  // a - boven
    3,  // b - rechtsboven
    2,  // c - rechtsonder
    1,  // d - onder
    7,  // e - linksonder
    5,  // f - linksboven
    6   // g - midden
};


// -----------------------------------------------------------------------------
// Dag-tientallen
// -----------------------------------------------------------------------------

static const uint8_t R2_DAY_TENS[7] = {
    11, // a
    10, // b
    9,  // c
    8,  // d
    14, // e
    12, // f
    13  // g
};


// -----------------------------------------------------------------------------
// Maand-eenheden
// -----------------------------------------------------------------------------

static const uint8_t R2_MONTH_ONES[7] = {
    19, // a
    18, // b
    17, // c
    16, // d
    22, // e
    20, // f
    21  // g
};


// Maand-tiental.
//
// Alleen "1" is nodig voor maanden 10-12.
static const uint8_t R2_MONTH_TENS_1_BOTTOM = 23;
static const uint8_t R2_MONTH_TENS_1_TOP = 24;


// Indicatoren.
static const uint8_t R2_IND_D = 0;
static const uint8_t R2_IND_M = 15;


// ============================================================================
// R11 - 20 LEDs
//
// Temperatuur:
//
//   0-1       = "1" bij 100..199
//   2-8       = eerste temperatuurcijfer
//   9-14,17   = tweede temperatuurcijfer
//   15        = Celsius
//   16        = Fahrenheit
//   18        = DST
//   19        = ongebruikt
// ============================================================================


// -----------------------------------------------------------------------------
// Eerste "1"
// -----------------------------------------------------------------------------

static const uint8_t R11_ONE_TOP = 0;
static const uint8_t R11_ONE_BOTTOM = 1;


// -----------------------------------------------------------------------------
// Eerste 8
//
// VOLLEDIG GERECONSTRUEERD
// -----------------------------------------------------------------------------

static const uint8_t R11_FIRST_8[7] = {
    6,  // a - boven
    7,  // b - rechtsboven
    8,  // c - rechtsonder
    3,  // d - onder
    2,  // e - linksonder
    5,  // f - linksboven
    4   // g - midden
};


// -----------------------------------------------------------------------------
// Tweede 8
//
// VOLLEDIG GERECONSTRUEERD
// -----------------------------------------------------------------------------

static const uint8_t R11_SECOND_8[7] = {
    13, // a - boven
    14, // b - rechtsboven
    17, // c - rechtsonder
    10, // d - onder
    9,  // e - linksonder
    12, // f - linksboven
    11  // g - midden
};


// Indicatoren.
static const uint8_t R11_CELSIUS = 15;
static const uint8_t R11_FAHRENHEIT = 16;
static const uint8_t R11_DST = 18;


// ============================================================================
// RENDER TIJD
// ============================================================================

inline void render_time(
    esphome::light::AddressableLight *r6,
    esphome::light::AddressableLight *r1,
    esphome::light::AddressableLight *r2,
    esphome::light::AddressableLight *r11,
    const esphome::ESPTime &time,
    float temperature_c,
    bool temperature_valid
) {

  if (!time.is_valid()) {
    return;
  }

  const esphome::Color off(0, 0, 0);
  const esphome::Color on = COLOR;


  // ==========================================================================
  // Alles eerst wissen
  // ==========================================================================

  clear_strip(*r6);
  clear_strip(*r1);
  clear_strip(*r2);
  clear_strip(*r11);


  // ==========================================================================
  // R6 - UREN
  // ==========================================================================

  if (SHOW_TIME) {

    const uint8_t hour_tens = time.hour / 10;
    const uint8_t hour_ones = time.hour % 10;

    // Eerste cijfer HH
    draw_2led_pairs(
        *r6,
        R6_HOUR_TENS,
        DIGIT[hour_tens],
        on
    );

    // Tweede cijfer HH
    draw_2led_start(
        *r6,
        R6_HOUR_ONES,
        DIGIT[hour_ones],
        on
    );


    // ------------------------------------------------------------------------
    // Weekdag
    // ------------------------------------------------------------------------

    if (SHOW_WEEKDAY &&
        time.day_of_week >= 1 &&
        time.day_of_week <= 7) {

      (*r6)[R6_WEEKDAY[time.day_of_week - 1]] = on;
    }


    // ------------------------------------------------------------------------
    // PM indicator
    //
    // 24-uursmodus:
    // bewust UIT laten.
    //
    // De klok gebruikt 00..23, dus PM is niet nodig.
    // ------------------------------------------------------------------------

    // (*r6)[R6_PM] = on;


    // ------------------------------------------------------------------------
    // DST indicator
    //
    // Deze zit volgens jouw metingen op R11.
    // Wordt onder R11 aangezet.
    // ------------------------------------------------------------------------
  }


  // ==========================================================================
  // R1 - MINUTEN
  // ==========================================================================

  if (SHOW_TIME) {

    const uint8_t minute_tens = time.minute / 10;
    const uint8_t minute_ones = time.minute % 10;

    draw_2led_pairs(
        *r1,
        R1_MIN_TENS,
        DIGIT[minute_tens],
        on
    );

    draw_2led_pairs(
        *r1,
        R1_MIN_ONES,
        DIGIT[minute_ones],
        on
    );


    // ------------------------------------------------------------------------
    // Dubbele punt
    // ------------------------------------------------------------------------

    bool colon_on = true;

    if (BLINK_COLON) {
      colon_on = (time.second % 2) == 0;
    }

    if (colon_on) {
      (*r1)[R1_COLON_TOP] = on;
      (*r1)[R1_COLON_BOTTOM] = on;
    }
  }


  // ==========================================================================
  // R2 - DATUM
  // ==========================================================================

  if (SHOW_DATE) {

    const uint8_t day_tens = time.day_of_month / 10;
    const uint8_t day_ones = time.day_of_month % 10;

    const uint8_t month_tens = time.month / 10;
    const uint8_t month_ones = time.month % 10;


    // ------------------------------------------------------------------------
    // D-indicator
    // ------------------------------------------------------------------------

    (*r2)[R2_IND_D] = on;


    // ------------------------------------------------------------------------
    // Dag
    // ------------------------------------------------------------------------

    draw_1led_segments(
        *r2,
        R2_DAY_TENS,
        DIGIT[day_tens],
        on
    );

    draw_1led_segments(
        *r2,
        R2_DAY_ONES,
        DIGIT[day_ones],
        on
    );


    // ------------------------------------------------------------------------
    // M-indicator
    // ------------------------------------------------------------------------

    (*r2)[R2_IND_M] = on;


    // ------------------------------------------------------------------------
    // Maand
    // ------------------------------------------------------------------------

    draw_1led_segments(
        *r2,
        R2_MONTH_ONES,
        DIGIT[month_ones],
        on
    );


    // Maand-tiental.
    //
    // 01..09 -> niets
    // 10..12 -> "1"
    //

    if (month_tens == 1) {
      (*r2)[R2_MONTH_TENS_1_TOP] = on;
      (*r2)[R2_MONTH_TENS_1_BOTTOM] = on;
    }
  }


  // ==========================================================================
  // R11 - TEMPERATUUR
  // ==========================================================================

  if (SHOW_TEMPERATURE &&
      temperature_valid &&
      std::isfinite(temperature_c)) {

    // Afronden naar hele graden.
    int temperature = static_cast<int>(
        std::lround(temperature_c)
    );


    // ------------------------------------------------------------------------
    // Alleen 0..199 °C ondersteunen.
    //
    // Voor normale binnentemperaturen is dit ruim voldoende.
    // ------------------------------------------------------------------------

    if (temperature >= 0 && temperature <= 199) {

      const int ones = temperature % 10;
      const int tens = (temperature / 10) % 10;
      const int hundreds = temperature / 100;


      // ----------------------------------------------------------------------
      // Honderdtal
      //
      // Alleen "1" is fysiek aanwezig.
      // ----------------------------------------------------------------------

      if (hundreds == 1) {
        (*r11)[R11_ONE_TOP] = on;
        (*r11)[R11_ONE_BOTTOM] = on;
      }


      // ----------------------------------------------------------------------
      // Tiental
      // ----------------------------------------------------------------------

      draw_1led_segments(
          *r11,
          R11_FIRST_8,
          DIGIT[tens],
          on
      );


      // ----------------------------------------------------------------------
      // Eenheid
      // ----------------------------------------------------------------------

      draw_1led_segments(
          *r11,
          R11_SECOND_8,
          DIGIT[ones],
          on
      );


      // ----------------------------------------------------------------------
      // Celsius
      // ----------------------------------------------------------------------

      (*r11)[R11_CELSIUS] = on;


      // ----------------------------------------------------------------------
      // Fahrenheit UIT
      // ----------------------------------------------------------------------

      // (*r11)[R11_FAHRENHEIT] = on;
    }
  }


  // ==========================================================================
  // DST
  // ==========================================================================

  if (time.is_dst) {
    (*r11)[R11_DST] = on;
  }


  // ==========================================================================
  // Verstuur alle gewijzigde buffers
  // ==========================================================================

  r6->schedule_show();
  r1->schedule_show();
  r2->schedule_show();
  r11->schedule_show();
}

}  // namespace klok
