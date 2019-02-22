/**
 *  Drums++
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2001-2019 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "general.h"
#include "Midi.h"

#define DIVISIONS 240

long marker;

int write_long_b(FILE *out, int n)
{
  putc(((n >> 24) & 0xff), out);
  putc(((n >> 16) & 0xff), out);
  putc(((n >> 8) & 0xff), out);
  putc((n & 0xff), out);

  return 0;
}

int write_word_b(FILE *out, int n)
{
  putc(((n >> 8) & 0xff), out);
  putc((n & 0xff), out);

  return 0;
}

void write_var(FILE *out, int i)
{
  int t, k;

  t = 7;

  while((i >> t) != 0)
  {
    t = t + 7;
  }

  t = t - 7;

  for (k = t; k >= 0; k = k - 7)
  {
    if (k != 0)
    {
      putc(((i >> k) & 127) + 128, out);
    }
      else
    {
      putc(((i >> k) & 127), out);
    }
  }
}

void write_midi_header(FILE *out)
{
  const char *i = "Created by Drums++ (http://dpp.mikekohn.net/).";

  fprintf(out,"MThd");
  write_long_b(out, 6);
  write_word_b(out, 0);
  write_word_b(out, 1);
  write_word_b(out, DIVISIONS);

  fprintf(out,"MTrk");
  marker = ftell(out);
  write_long_b(out, 0);

  if (song_name[0]!=0)
  {
    write_var(out, 0);
    putc(0xff, out);
    putc(0x03, out);
    write_var(out, strlen((char *)song_name));
    fprintf(out,"%s", song_name);
  }

  write_var(out, 0);
  putc(0xff, out);
  putc(0x01, out);
  write_var(out, strlen(i));
  fprintf(out,"%s", i);

  /* write_midi_bpm(out); */
}

void write_midi_note(FILE *out, struct note_t *note)
{
  int d;

  d = (int)((float)DIVISIONS * ((float)note->duration / (float)(60000000 / song_info.bpm)));

  write_var(out, 0);
  putc(0x90 + note->midi_channel, out);

  putc(note->instrument, out);
  putc(note->volume, out);

  /* if (d != 0 || 1 == 1) */
  {
    write_var(out, d);
    putc(0x80 + note->midi_channel, out);
    putc(note->instrument, out);
    /* putc(0, out); */
    putc(64, out);
  }
}

void write_midi_footer(FILE *out)
{
  int i;

  write_var(out, 0);
  putc(0xff, out);
  putc(0x2f, out);
  putc(0x00, out);

  i = ftell(out);
  fseek(out, marker, 0);
  write_long_b(out, (i - marker) - 4);
  fseek(out, i, 0);
}

void write_midi_bpm(FILE *out)
{
  int d;

  write_var(out, 0);
  putc(0xff, out);
  putc(0x51, out);
  putc(0x03, out);
  d = 60000000 / song_info.bpm;
  putc(d >> 16, out);
  putc((d >> 8) & 0xff,out);
  putc(d & 0xff, out);
}

void write_midi_timesignature(FILE *out)
{
  int d;

  d = song_info.timesignature_base;

  if (d == 32) { d = 5; }
    else
  if (d == 16) { d = 4; }
    else
  if (d == 8) { d = 3; }
    else
  if (d == 4) { d = 2; }
    else
  if (d == 2) { d = 1; }
    else
  if (d == 1) { d = 0; }
    else
  { return; }

  write_var(out, 0);
  putc(0xff, out);
  putc(0x58, out);
  putc(0x04, out);

  putc(song_info.timesignature_beats, out);
  putc(d, out);

  if (d == 3)
  {
    putc(DIVISIONS / 3, out);
  }
    else
  {
    putc(DIVISIONS, out);
  }

  putc(8, out);
}
