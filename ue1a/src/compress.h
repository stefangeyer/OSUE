/**
 * @file compress.h
 * @author Stefan Geyer <stefan.geyer@student.tuwien.ac.at>
 * @date 06.03.2018
 *
 * @brief Compression header.
 *
 * Defines compression functions.
 **/

#ifndef UE1A_COMPRESS_H
#define UE1A_COMPRESS_H

/**
 * Compress a string's content
 * @brief Compresses the src string and replaces any sequences of chars with their short representation
 * @details Example: The sequence aaaaabbb would be replaced with a5b3
 * @param src The string to compress
 * @param res The result string
 * @param n The max size of result
 * @return The result's size
 */
int strcmpr(const char *src, char *res, size_t n);

#endif //UE1A_COMPRESS_H
