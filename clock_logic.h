#pragma once

#include "esphome.h"
#include <algorithm>
#include <cmath>

namespace klok {

// ============================================================================
// INSTELLINGEN
// ============================================================================

static const bool SHOW_TIME = true;
static const bool SHOW_WEEKDAY = true;
static const bool SHOW_DATE = true;
static const bool SHOW_TEMPERATURE = true;

// Dubbele punt:
// false = continu aan
// true  = iedere seconde knipperen
static const bool BLINK_COLON = false;

// ============================================================================
// 7-segment cijfers
// bit 0 = a, bit 1 = b, bit 2 = c, bit 3 = d,
// bit 4 = e, bit 5 = f, bit 6 = g
// ============================================================================

static const uint8_t DIGIT[10] = {
    0x3F,  // 0
    0x06,  // 1
    0x5B,  // 2
    0x4F,  // 3
    0x66,  // 4
    0x6D,  // 5
    0x7D,  // 6
    0x07,  // 7
    0x7F,  // 8
    0x6F   // 9
};

inline esphome::Color make_color(float red, float green, float blue) {
  red = std::max(0.0f, std::min(1.0f, red));
  green = std::max(0.0f, std::min(1.0f, green));
  blue = std::max(0.0f, std::min(1.0f, blue));

  return esphome::Color(
      static_cast<uint8_t>(std::lround(red * 255.0f)),
      static_cast<uint8_t>(std::lround(green * 255.0f)),
      static_cast<uint8_t>(std::lround(blue * 255.0f))
  );
}

inline void clear_strip(esphome::light::AddressableLight &strip) {
  for (int i = 0; i < strip.size(); i++) {
    strip[i] = esphome::Color::BLACK;
  }
}

inline void draw_2led_start(
    esphome::light::AddressableLight &strip,
    const uint8_t starts[7],
    uint8_t mask,
    const esphome::Color &color) {
  for (int segment = 0; segment < 7; segment++) {
    if (mask & (1 << segment)) {
      strip[starts[segment]] = color;
      strip[starts[segment] + 1] = color;
    }
  }
}

inline void draw_2led_pairs(
    esphome::light::AddressableLight &strip,
    const uint8_t pairs[7][2],
    uint8_t mask,
    const esphome::Color &color) {
  for (int segment = 0; segment < 7; segment++) {
    if (mask & (1 << segment)) {
      strip[pairs[segment][0]] = color;
      strip[pairs[segment][1]] = color;
    }
  }
}

inline void draw_1led_segments(
    esphome::light::AddressableLight &strip,
    const uint8_t segments[7],
    uint8_t mask,
    const esphome::Color &color) {
  for (int segment = 0; segment < 7; segment++) {
    if (mask & (1 << segment)) {
      strip[segments[segment]] = color;
    }
  }
}

// ============================================================================
// R6 - 36 LEDs: HH + weekdag + PM
// ============================================================================

static const uint8_t R6_HOUR_ONES[7] = {
    0, 2, 4, 8, 10, 12, 6
};

static const uint8_t R6_HOUR_TENS[7][2] = {
    {23, 24},  // a
    {14, 15},  // b
    {16, 17},  // c
    {18, 19},  // d
    {25, 26},  // e
    {27, 28},  // f
    {20, 21}   // g
};

static const uint8_t R6_PM = 22;

static const uint8_t R6_WEEKDAY[7] = {
    29,  // Sun
    35,  // Mon
    34,  // Tue
    33,  // Wed
    32,  // Thu
    31,  // Fri
    30   // Sat
};

// ============================================================================
// R1 - 32 LEDs: MM + dubbele punt + alarm
// ============================================================================

static const uint8_t R1_MIN_TENS[7][2] = {
    {3, 4},
    {5, 6},
    {23, 24},
    {27, 28},
    {29, 30},
    {1, 2},
    {25, 26}
};

static const uint8_t R1_MIN_ONES[7][2] = {
    {9, 10},
    {11, 12},
    {15, 16},
    {17, 18},
    {21, 22},
    {7, 8},
    {19, 20}
};

static const uint8_t R1_COLON_TOP = 0;
static const uint8_t R1_COLON_BOTTOM = 31;
static const uint8_t R1_ALARM2 = 13;
static const uint8_t R1_ALARM1 = 14;

// ============================================================================
// R2 - 25 LEDs: DD + MM
// ============================================================================

static const uint8_t R2_DAY_ONES[7] = {
    4, 3, 2, 1, 7, 5, 6
};

static const uint8_t R2_DAY_TENS[7] = {
    11, 10, 9, 8, 14, 12, 13
};

static const uint8_t R2_MONTH_ONES[7] = {
    19, 18, 17, 16, 22, 20, 21
};

static const uint8_t R2_MONTH_TENS_1_BOTTOM = 23;
static const uint8_t R2_MONTH_TENS_1_TOP = 24;

static const uint8_t R2_IND_D = 0;
static const uint8_t R2_IND_M = 15;

// ============================================================================
// R11 - 20 LEDs: temperatuur + C/F + DST
// ============================================================================

static const uint8_t R11_ONE_TOP = 0;
static const uint8_t R11_ONE_BOTTOM = 1;

static const uint8_t R11_FIRST_8[7] = {
    6,  // a
    7,  // b
    8,  // c
    3,  // d
    2,  // e
    5,  // f
    4   // g
};

static const uint8_t R11_SECOND_8[7] = {
    13, // a
    14, // b
    17, // c
    10, // d
    9,  // e
    12, // f
    11  // g
};

static const uint8_t R11_CELSIUS = 15;
static const uint8_t R11_FAHRENHEIT = 16;
static const uint8_t R11_DST = 18;

// ============================================================================
// VOLLEDIGE RENDERER
// ============================================================================

inline void render_time(
    esphome::light::AddressableLight *r6,
    esphome::light::AddressableLight *r1,
    esphome::light::AddressableLight *r2,
    esphome::light::AddressableLight *r11,
    const esphome::ESPTime &time,
    float red,
    float green,
    float blue,
    bool light_on,
    bool show_dst,
    float temperature_c,
    bool temperature_valid) {

  if (!time.is_valid()) {
    return;
  }

  if (!light_on) {
    clear_strip(*r6);
    clear_strip(*r1);
    clear_strip(*r2);
    clear_strip(*r11);

    r6->schedule_show();
    r1->schedule_show();
    r2->schedule_show();
    r11->schedule_show();
    return;
  }

  const esphome::Color color = make_color(red, green, blue);

  clear_strip(*r6);
  clear_strip(*r1);
  clear_strip(*r2);
  clear_strip(*r11);

  // --------------------------------------------------------------------------
  // R6 - HH
  // --------------------------------------------------------------------------
  if (SHOW_TIME) {
    const uint8_t hour_tens = time.hour / 10;
    const uint8_t hour_ones = time.hour % 10;

    draw_2led_pairs(*r6, R6_HOUR_TENS, DIGIT[hour_tens], color);
    draw_2led_start(*r6, R6_HOUR_ONES, DIGIT[hour_ones], color);
  }

  // Weekdag
  if (SHOW_WEEKDAY && time.day_of_week >= 1 && time.day_of_week <= 7) {
    (*r6)[R6_WEEKDAY[time.day_of_week - 1]] = color;
  }

  // PM-indicator is bewust uit omdat de klok in 24-uursmodus werkt.

  // --------------------------------------------------------------------------
  // R1 - MM + dubbele punt
  // --------------------------------------------------------------------------
  if (SHOW_TIME) {
    const uint8_t minute_tens = time.minute / 10;
    const uint8_t minute_ones = time.minute % 10;

    draw_2led_pairs(*r1, R1_MIN_TENS, DIGIT[minute_tens], color);
    draw_2led_pairs(*r1, R1_MIN_ONES, DIGIT[minute_ones], color);

    bool colon_on = true;
    if (BLINK_COLON) {
      colon_on = (time.second % 2) == 0;
    }

    if (colon_on) {
      (*r1)[R1_COLON_TOP] = color;
      (*r1)[R1_COLON_BOTTOM] = color;
    }
  }

  // --------------------------------------------------------------------------
  // R2 - DD + MM
  // --------------------------------------------------------------------------
  if (SHOW_DATE) {
    const uint8_t day_tens = time.day_of_month / 10;
    const uint8_t day_ones = time.day_of_month % 10;
    const uint8_t month_tens = time.month / 10;
    const uint8_t month_ones = time.month % 10;

    (*r2)[R2_IND_D] = color;

    draw_1led_segments(*r2, R2_DAY_TENS, DIGIT[day_tens], color);
    draw_1led_segments(*r2, R2_DAY_ONES, DIGIT[day_ones], color);

    (*r2)[R2_IND_M] = color;

    draw_1led_segments(*r2, R2_MONTH_ONES, DIGIT[month_ones], color);

    if (month_tens == 1) {
      (*r2)[R2_MONTH_TENS_1_BOTTOM] = color;
      (*r2)[R2_MONTH_TENS_1_TOP] = color;
    }
  }

  // --------------------------------------------------------------------------
  // R11 - temperatuur
  // --------------------------------------------------------------------------
  if (SHOW_TEMPERATURE && temperature_valid && std::isfinite(temperature_c)) {
    const int temperature = static_cast<int>(std::lround(temperature_c));

    if (temperature >= 0 && temperature <= 199) {
      const int ones = temperature % 10;
      const int tens = (temperature / 10) % 10;
      const int hundreds = temperature / 100;

      if (hundreds == 1) {
        (*r11)[R11_ONE_TOP] = color;
        (*r11)[R11_ONE_BOTTOM] = color;
      }

      draw_1led_segments(*r11, R11_FIRST_8, DIGIT[tens], color);
      draw_1led_segments(*r11, R11_SECOND_8, DIGIT[ones], color);

      (*r11)[R11_CELSIUS] = color;
    }
  }

  // --------------------------------------------------------------------------
  // DST
  // --------------------------------------------------------------------------
  if (show_dst && time.is_dst) {
    (*r11)[R11_DST] = color;
  }

  r6->schedule_show();
  r1->schedule_show();
  r2->schedule_show();
  r11->schedule_show();
}

}  // namespace klok
