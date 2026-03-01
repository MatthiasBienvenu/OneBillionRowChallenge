/* faster implementation of strtof but does not comply with all IEEE 754
 * standard. This function can just parse floats that are in the format
 * -?(\d+)?(\.(\d+)?)? so no nan nor infinity
 *
 * return the parsed float or 0.0f if no number could be parsed
 */
float fast_strtof(const char *restrict string, char **restrict end);
