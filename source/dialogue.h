#pragma once

#include <filesystem.h>
#include <math.h>
#include <maxmod9.h>
#include <nds.h>
#include <nf_lib.h>
#include <stdio.h>
#include <time.h>

#include <algorithm>

#include "camera.h"
#include "enums.h"
#include "menu.h"
#include "soundbank.h"
#include "transition.h"
#include "usefulfunctions.h"

static int dialogueIt;
static bool inDialogue;
static const int max_chars = 20;

static const char cornermenDialogue[6][4][5][20] = {
    {
        // ROONEY
        {
            // DOING WELL
            "That's it,",
            "kid!",
            "Keep up and",
            "don't lose",
            "focus!",
        },
        {
            // GETTING HURT
            "C'mon, be a",
            "boxer, kid!",
            "Block.",
            "  Dodge.",
            "    Punch!!",
        },
        {
            // GETTING TIRED
            "Doing good,",
            "but listen:",
            "Don't tire ",
            "yourself,",
            "kid!",
        },
        {
            // DECISION
            "Well kid,",
            "Let's hope",
            "the score-",
            "cards",
            "like you.",
        },
    },
    {
        // SLIM JIM
        {
            // DOING WELL
            "Okay, okay!",
            "Clean",
            "punches,",
            "Knock that",
            "sucker out!",
        },
        {
            // GETTING HURT
            "Dont eat",
            "those! Move",
            "and use",
            "your range,",
            "compa!",
        },
        {
            // GETTING TIRED
            "Calm down!",
            "Guarda la",
            "juerza,",
            "Punch",
            "smart!",
        },
        {
            // DECISION
            "I hope you,",
            "punch that",
            "fool enough",
            "for the win",
            "...",
        },
    },
    {
        // JILL
        {
            // DOING WELL
            "Meow! Meow!",
            "(You got",
            "it, now",
            "out-box",
            "this fool!",
        },
        {
            // GETTING HURT
            "Meow!",
            "(Reckless!",
            "punch and",
            "stay away!)",
            "",
        },
        {
            // GETTING TIRED
            "Meow?",
            "(Too happy,",
            "too much",
            "energy,",
            "calm down!)",
        },
        {
            // DECISION
            "Meow..",
            "(Looks like",
            "you're",
            "winning by",
            "decision)",
        },
    },
    {
        // PABLO
        {
            // DOING WELL
            "Esta pelea!",
            "es tuya!",
            "Stay quick",
            "and sharp",
            "mijo!",
        },
        {
            // GETTING HURT
            "Defend!",
            "Block and",
            "dodge more,",
            "Dont eat",
            "the hits!",
        },
        {
            // GETTING TIRED
            "Dont throw-",
            "yourself",
            "carelessly",
            "Respira y",
            "recupera!",
        },
        {
            // DECISION
            "Let's hope",
            "the judges",
            "like you,",
            "amigo.",
            "",
        },
    },
    {
        // SVETLANA
        {
            // DOING WELL
            "Wunderbar,",
            "bol'shoy!!",
            "Put that",
            "durak",
            "to sleep!",

        },
        {
            // GETTING HURT
            "Mein gott!!",
            "Be more",
            "careful ja?",
            "Defend then",
            "press them!",
        },
        {
            // GETTING TIRED
            "Don't let",
            "them dizzy",
            "you! Dent",
            "them at the",
            "right time!",
        },
        {
            // DECISION
            "Good job!",
            "Judges",
            "better",
            "say you",
            "won it!",
        },
    },
    {
        // KAIRO
        {
            // DOING WELL
            "Looking",
            "hot, champ.",
            "Go on then-",
            "Put that",
            "clown down.",
        },
        {
            // GETTING HURT
            "Getting",
            "too cocky.",
            "They wanna",
            "fight, You",
            "wanna goof?",
        },
        {
            // GETTING TIRED
            "Ya fadin'",
            "Manage, and",
            "keep the",
            "pressure on",
            "them!",
        },
        {
            // DECISION
            "Judges will",
            "call it.",
            "",
            "Don't drop",
            "your guard",
        },
    },
};

void StartDialogue();
void StopDialogue();

void RunDialogue(const char dial_script[][max_chars], int lines, int speed, int x, int y);