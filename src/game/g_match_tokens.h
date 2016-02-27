/* C++ code produced by gperf version 3.0.3 */
/* Command-line: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/gperf -t7C --language=C++ input_tokens.gperf  */
/* Computed positions: -k'1-2' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

#line 1 "input_tokens.gperf"

#include <string.h>
#include "g_strparse.h"
#line 5 "input_tokens.gperf"
struct g_strtoken_t { char* name; g_StringToken_t index; };

#define TOTAL_KEYWORDS 48
#define MIN_WORD_LENGTH 1
#define MAX_WORD_LENGTH 20
#define MIN_HASH_VALUE 1
#define MAX_HASH_VALUE 92
/* maximum key range = 92, duplicates = 0 */

class Perfect_Hash
{
private:
	static inline unsigned int hash(const char *str, unsigned int len);
public:
	static const struct g_strtoken_t *in_word_set(const char *str, unsigned int len);
};

inline unsigned int
Perfect_Hash::hash(register const char *str, register unsigned int len)
{
	static const unsigned char asso_values[] =
	{
		93, 93, 93, 93, 93, 93, 93, 93, 93, 93,
		93, 93, 93, 93, 93, 93, 93, 93, 93, 93,
		93, 93, 93, 93, 93, 93, 93, 93, 93, 93,
		93, 93, 93, 93, 93, 93, 93, 93, 93, 93,
		93, 93, 93, 93, 93, 93, 93, 93, 93, 93,
		93, 93, 93, 93, 93, 93, 93, 93, 93, 93,
		93, 93, 93, 93, 93, 93, 93, 93, 93, 93,
		93, 93, 93, 93, 93, 93, 93, 93, 93, 93,
		93, 93, 93, 93, 93, 93, 93, 93, 93, 93,
		93, 93, 93, 93, 93,  0, 93,  0,  0,  5,
		45, 50, 50, 15, 16, 50, 93, 15, 10,  0,
		30, 93, 10, 93, 35, 25, 40,  0, 93, 45,
		15, 93, 93, 93, 93, 93, 93, 93
	};
	register unsigned int hval = len;

	switch (hval)
	{
	default:
		hval += asso_values[(unsigned char)str[1]];
		/*FALLTHROUGH*/
	case 1:
		hval += asso_values[(unsigned char)str[0]];
		break;
	}
	return hval;
}

const struct g_strtoken_t *
Perfect_Hash::in_word_set(register const char *str, register unsigned int len)
{
	static const struct g_strtoken_t wordlist[] =
	{
		{ "" },
#line 29 "input_tokens.gperf"
	{ "m",								TOK_m },
#line 54 "input_tokens.gperf"
	{ "mu",								TOK_mu },
	{ "" },{ "" },{ "" },
#line 26 "input_tokens.gperf"
	{ "c",								TOK_c },
#line 44 "input_tokens.gperf"
	{ "bc",								TOK_bc },
#line 45 "input_tokens.gperf"
	{ "bcs",							TOK_bcs },
	{ "" },{ "" },
#line 43 "input_tokens.gperf"
	{ "l",								TOK_l },
#line 46 "input_tokens.gperf"
	{ "bl",								TOK_bl },
#line 47 "input_tokens.gperf"
	{ "bls",							TOK_bls },
	{ "" },{ "" },
#line 52 "input_tokens.gperf"
	{ "g",								TOK_g },
#line 37 "input_tokens.gperf"
	{ "lc",								TOK_lc },
#line 15 "input_tokens.gperf"
	{ "password",						TOK_password },
	{ "" },
#line 20 "input_tokens.gperf"
	{ "pmove_msec",						TOK_pmove_msec },
#line 19 "input_tokens.gperf"
	{ "pmove_fixed",					TOK_pmove_fixed },
#line 14 "input_tokens.gperf"
	{ "cl_guid",						TOK_cl_guid },
#line 21 "input_tokens.gperf"
	{ "ch",								TOK_ch },
#line 9 "input_tokens.gperf"
	{ "gamestate",						TOK_gamestate },
	{ "" },
#line 30 "input_tokens.gperf"
	{ "s",								TOK_s },
#line 31 "input_tokens.gperf"
	{ "xp",								TOK_xp },
#line 18 "input_tokens.gperf"
	{ "cg_uinfo",						TOK_cg_uinfo },
#line 7 "input_tokens.gperf"
	{ "g_currentRound",					TOK_g_currentRound },
#line 8 "input_tokens.gperf"
	{ "g_nextTimeLimit",				TOK_g_nextTimeLimit },
#line 24 "input_tokens.gperf"
	{ "n",								TOK_n },
#line 10 "input_tokens.gperf"
	{ "g_currentCampaign",				TOK_g_currentCampaign },
	{ "" },
#line 13 "input_tokens.gperf"
	{ "name",							TOK_name },
#line 11 "input_tokens.gperf"
	{ "g_currentCampaignMap",			TOK_g_currentCampaignMap },
#line 27 "input_tokens.gperf"
	{ "r",								TOK_r },
#line 48 "input_tokens.gperf"
	{ "br",								TOK_br },
#line 49 "input_tokens.gperf"
	{ "brs",							TOK_brs },
#line 23 "input_tokens.gperf"
	{ "rate",							TOK_rate },
#line 16 "input_tokens.gperf"
	{ "scriptName",						TOK_scriptName },
#line 25 "input_tokens.gperf"
	{ "t",								TOK_t },
#line 53 "input_tokens.gperf"
	{ "gs",								TOK_gs },
#line 51 "input_tokens.gperf"
	{ "hs",								TOK_hs },
	{ "" },
#line 22 "input_tokens.gperf"
	{ "skill",							TOK_skill },
#line 35 "input_tokens.gperf"
	{ "w",								TOK_w },
#line 41 "input_tokens.gperf"
	{ "bd",								TOK_bd },
	{ "" },{ "" },{ "" },
#line 28 "input_tokens.gperf"
	{ "f",								TOK_f },
#line 33 "input_tokens.gperf"
	{ "dc",								TOK_dc },
	{ "" },{ "" },{ "" },{ "" },
#line 36 "input_tokens.gperf"
	{ "lw",								TOK_lw },
	{ "" },{ "" },{ "" },{ "" },
#line 12 "input_tokens.gperf"
	{ "ip",								TOK_ip },
#line 42 "input_tokens.gperf"
	{ "hd",								TOK_hd },
	{ "" },{ "" },{ "" },
#line 40 "input_tokens.gperf"
	{ "rn",								TOK_rn },
#line 50 "input_tokens.gperf"
	{ "he",								TOK_he },
	{ "" },{ "" },{ "" },
#line 38 "input_tokens.gperf"
	{ "sw",								TOK_sw },
	{ "" },{ "" },{ "" },{ "" },
#line 32 "input_tokens.gperf"
	{ "dn",								TOK_dn },
	{ "" },{ "" },{ "" },{ "" },
#line 34 "input_tokens.gperf"
	{ "dr",								TOK_dr },
	{ "" },{ "" },{ "" },{ "" },{ "" },
#line 39 "input_tokens.gperf"
	{ "ref",							TOK_ref },
	{ "" },{ "" },{ "" },
#line 17 "input_tokens.gperf"
	{ "respawn",						TOK_respawn }
	};

	if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
	{
		unsigned int key = hash(str, len);

		if (key <= MAX_HASH_VALUE)
		{
			register const char *s = wordlist[key].name;

			if (*str == *s && !strcmp(str + 1, s + 1))
				return &wordlist[key];
		}
	}
	return 0;
}
#line 55 "input_tokens.gperf"

