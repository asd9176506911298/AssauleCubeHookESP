#pragma once
#include <Windows.h>
#include "geom.h"

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

enum gameModes
{
    TEAMDEATHMATCH = 0,
    COOPEDIT,
    DEATHMATCH,
    SURVIVOR,
    TEAMSURVIVOR,
    CTF,
    PISTOLFRENZY,
    BOTTEAMDEATHMATCH,
    BOTDEATHMATCH,
    LASTSWISSSTANDING,
    ONESHOTONEKILL,   
    TEAMONESHOTONEKILL,
    BOTONESHOTONEKILL,
    HUNTTHEFLAG,
    TEAMKEEPTHEFLAG,
    KEEPTHEFLAG,
    TEAMPF,
    TEAMLSS,
    BOTPISTOLFRENZY,
    BOTLSS,
    BOTTEAMSURVIVOR,
    BOTTEAMONESHOTONKILL,
    NUM
};

class ent
{
public:

    // be sure to put all the offsets you pad this way in a union
    union
    {
        //              Type     Name    Offset
        DEFINE_MEMBER_N(vec3, head, 0x4);
        DEFINE_MEMBER_N(vec3, pos, 0x34);
        DEFINE_MEMBER_N(int, index, 0x1e4);
        DEFINE_MEMBER_N(char, name[16], 0x225);
        DEFINE_MEMBER_N(int, team, 0x32C);
    };
};

struct entList
{
    ent* ents[31];
};