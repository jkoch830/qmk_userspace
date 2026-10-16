# Iris LM — `steno_qwerty` keymap

Visual reference for the current binds. Blank cells are transparent (`_______`) and fall through to the layer below.

**Modes:** QWERTY (default) ↔ Steno via chord **Cmd + A + S + D + F**.  
**RGB:** Backspace LED orange = QWERTY; Backspace + `0` LEDs orange = Steno. All other LEDs stay off.

---

## Base — QWERTY

<div style="background:#cfe8f0;padding:1rem 1.25rem 1.5rem;border-radius:8px;font-family:ui-monospace,Menlo,Consolas,monospace;font-size:13px;line-height:1.35;overflow-x:auto">

<pre style="margin:0;white-space:pre">
┌──────┬──────┬──────┬──────┬──────┬──────┐                    ┌──────┬──────┬──────┬──────┬──────┬──────┐
│ Esc  │  1   │  2   │  3   │  4   │  5   │                    │  6   │  7   │  8   │  9   │  0   │ Bksp │ ← orange LED (QWERTY)
│  ~   │  !   │  @   │  #   │  $   │  %   │                    │  ^   │  &   │  *   │  (   │  )   │      │
├──────┼──────┼──────┼──────┼──────┼──────┤                    ├──────┼──────┼──────┼──────┼──────┼──────┤
│ Tab  │  Q   │  W   │  E   │  R   │  T   │                    │  Y   │  U   │  I   │  O   │  P   │ Del  │
├──────┼──────┼──────┼──────┼──────┼──────┤                    ├──────┼──────┼──────┼──────┼──────┼──────┤
│ Ctrl │  A   │  S   │  D   │  F   │  G   │                    │  H   │  J   │  K   │  L   │  ;   │  '   │
├──────┼──────┼──────┼──────┼──────┼──────┼──────┐      ┌──────┼──────┼──────┼──────┼──────┼──────┼──────┤
│Shift │  Z   │  X   │  C   │  V   │  B   │ Home │      │ End  │  N   │  M   │  ,   │  .   │  /   │Shift │
└──────┴──────┴──────┴───┬──┴───┬──┴───┬──┴───┬──┘      └──┬───┴──┬───┴──┬───┴──┬───┴──────┴──────┴──────┘
                         │ Cmd  │Lower │Enter │            │Space │Upper │ Alt  │
                         │      │ Fn1  │      │            │      │ Fn2  │ Opt  │
                         └──────┴──────┴──────┘            └──────┴──────┴──────┘
</pre>

</div>

---

## Lower — hold **Fn1** (left thumb)

<div style="background:#f5d0b0;padding:1rem 1.25rem 1.5rem;border-radius:8px;font-family:ui-monospace,Menlo,Consolas,monospace;font-size:13px;line-height:1.35;overflow-x:auto">

<pre style="margin:0;white-space:pre">
┌──────┬──────┬──────┬──────┬──────┬──────┐                    ┌──────┬──────┬──────┬──────┬──────┬──────┐
│  `   │  !   │  @   │  #   │  $   │  %   │                    │  ^   │  &amp;   │  *   │  [   │  ]   │ PgUp │
│      │      │      │      │      │      │                    │      │      │      │Shift→│Shift→│      │
│      │      │      │      │      │      │                    │      │      │      │  {   │  }   │      │
├──────┼──────┼──────┼──────┼──────┼──────┤                    ├──────┼──────┼──────┼──────┼──────┼──────┤
│  ~   │      │  ↑   │      │ Boot │      │                    │      │  P7  │  P8  │  P9  │  P0  │ PgDn │
├──────┼──────┼──────┼──────┼──────┼──────┤                    ├──────┼──────┼──────┼──────┼──────┼──────┤
│ Ctrl │  ←   │  ↓   │  →   │      │  [   │                    │  ←   │  ↓   │  ↑   │  →   │  +   │  |   │
│      │      │      │      │      │      │                    │  h   │  j   │  k   │  l   │      │      │
├──────┼──────┼──────┼──────┼──────┼──────┼──────┐      ┌──────┼──────┼──────┼──────┼──────┼──────┼──────┤
│Shift │EE_CLR│      │      │      │  {   │  (   │      │  )   │  }   │  P1  │  P2  │  P3  │  -   │Shift │
└──────┴──────┴──────┴───┬──┴───┬──┴───┬──┴───┬──┘      └──┬───┴──┬───┴──┬───┴──┬───┴──────┴──────┴──────┘
                         │      │Lower │      │            │Word  │      │  P0  │
                         │      │ Fn1  │      │            │ Mod* │      │      │
                         └──────┴──────┴──────┘            └──────┴──────┴──────┘

* Word Mod (Space): Opt on Mac / Ctrl on Win·Linux — chord with ←/→ (or hjkl) for word hops
  e.g. Lower + Space Mod + h/l  →  move by word
</pre>

</div>

---

## Raise — hold **Fn2** (right thumb)

<div style="background:#d4c4e8;padding:1rem 1.25rem 1.5rem;border-radius:8px;font-family:ui-monospace,Menlo,Consolas,monospace;font-size:13px;line-height:1.35;overflow-x:auto">

<pre style="margin:0;white-space:pre">
┌──────┬──────┬──────┬──────┬──────┬──────┐                    ┌──────┬──────┬──────┬──────┬──────┬──────┐
│ F12  │  F1  │  F2  │  F3  │  F4  │  F5  │                    │  F6  │  F7  │  F8  │  F9  │ F10  │ F11  │
├──────┼──────┼──────┼──────┼──────┼──────┤                    ├──────┼──────┼──────┼──────┼──────┼──────┤
│      │  !   │  @   │  #   │  $   │  %   │                    │  ^   │  &amp;   │  *   │  (   │  )   │ Boot │
├──────┼──────┼──────┼──────┼──────┼──────┤                    ├──────┼──────┼──────┼──────┼──────┼──────┤
│      │ Prev │ Next │ Vol+ │ PgUp │  _   │                    │  =   │ Home │      │      │      │  \   │
│      │Track │Track │      │      │      │                    │      │      │      │      │      │      │
├──────┼──────┼──────┼──────┼──────┼──────┬──────┐      ┌──────┼──────┼──────┼──────┼──────┼──────┼──────┤
│ Mute │ Stop │ Play │ Vol− │ PgDn │  -   │  (   │      │  +   │ End  │      │      │      │EE_CLR│
└──────┴──────┴──────┴───┬──┴───┬──┴───┬──┴───┬──┘      └──┬───┴──┬───┴──┬───┴──┬───┴──────┴──────┴──────┘
                         │      │      │      │            │      │Raise │      │
                         │      │      │      │            │      │ Fn2  │      │
                         └──────┴──────┴──────┘            └──────┴──────┴──────┘


RGB mode / hue / sat / val keys removed — lighting is indicators-only.
</pre>

</div>

---

## Steno mode — Lapwing / GeminiPR

Default-layer swap (not a momentary layer). Same physical board; steno keys talk to Plover.

Toggle: hold **Cmd + A + S + D + F** together (works from QWERTY or Steno).

<div style="background:#c8e6c9;padding:1rem 1.25rem 1.5rem;border-radius:8px;font-family:ui-monospace,Menlo,Consolas,monospace;font-size:13px;line-height:1.35;overflow-x:auto">

<pre style="margin:0;white-space:pre">
┌──────┬──────┬──────┬──────┬──────┬──────┐                    ┌──────┬──────┬──────┬──────┬──────┬──────┐
│ Esc  │  1   │  2   │  3   │  4   │  5   │                    │  6   │  7   │  8   │  9   │  0   │ Bksp │ ← both LEDs orange
│  ~   │      │      │      │      │      │                    │      │      │      │      │      │      │
├──────┼──────┼──────┼──────┼──────┼──────┤                    ├──────┼──────┼──────┼──────┼──────┼──────┤
│ Tab  │  #   │  T-  │  P-  │  H-  │  *1  │                    │  *3  │  -F  │  -P  │  -L  │  -T  │  -D  │
│      │ (S1) │      │      │      │      │                    │      │      │      │      │      │      │
├──────┼──────┼──────┼──────┼──────┼──────┤                    ├──────┼──────┼──────┼──────┼──────┼──────┤
│ Ctrl │  S   │  K-  │  W-  │  R-  │  *2  │                    │  *4  │  -R  │  -B  │  -G  │  -S  │  -Z  │
│      │ (S2) │      │      │      │      │                    │      │      │      │      │      │      │
├──────┼──────┼──────┼──────┼──────┼──────┬──────┐      ┌──────┼──────┼──────┼──────┼──────┼──────┼──────┤
│Shift │  Z   │  X   │  C   │  V   │  B   │ Home │      │ End  │  N   │  M   │  ,   │  .   │  /   │Shift │
└──────┴──────┴──────┴───┬──┴───┬──┴───┬──┴───┬──┘      └──┬───┴──┬───┴──┬───┴──┬───┴──────┴──────┴──────┘
                         │ Cmd  │  A   │  O   │            │  E   │  U   │ Alt  │
                         └──────┴──────┴──────┘            └──────┴──────┴──────┘

Unlabeled letter keys (Z X C V B / N M , . / and number row) stay normal QWERTY.
# is Gemini S1 — Plover Lapwing usually remaps S1 → #.
</pre>

</div>

---

## Quick reference

| Action | How |
|--------|-----|
| Lower symbols / nav | Hold left **Fn1** |
| Raise F-keys / media | Hold right **Fn2** |
| Word-move (Mac/Win) | **Lower + Space + ←/→** (or hjkl arrows) |
| `[` `]` then `{` `}` | **Lower + 9/0**; add **Shift** for braces |
| Vim arrows | **Lower + h j k l** |
| Bootloader | **Lower + R** (left) or **Raise + Del** (right) |
| Clear EEPROM | **Lower + Z** or **Raise + RShift** |
| Toggle Steno ↔ QWERTY | **Cmd + A + S + D + F** |

**Tri-layer note:** Holding Lower **and** Upper together briefly activates layer 3 (the Steno *keymap* slot used as QMK’s “adjust” layer). Prefer the combo above for real Steno mode.
