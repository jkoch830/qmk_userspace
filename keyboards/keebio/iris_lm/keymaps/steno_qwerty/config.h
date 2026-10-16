// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

// Enable NKRO on every boot (modern QMK; replaces deprecated FORCE_NKRO).
#define NKRO_DEFAULT_ON true

// Five-key chord needs a slightly longer window than the default 50ms.
#define COMBO_TERM 200

// Prefer GeminiPR even if EEPROM previously stored another protocol.
#define STENO_DEFAULT_MODE STENO_MODE_GEMINI
