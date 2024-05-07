/**
 * @file digital_pot.c
 * @author astro
 * 
 * @copyright Copyright (c) 2024
 * 
*/

#if defined(USE_MCP4017)
#   include "mcp4017.c"
#else
#   error "digital pot not available"
#endif