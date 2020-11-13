/**
 * @file attribs.h
 * @author TheSomeMan
 * @date 2020-10-20
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#ifndef ATTRIBS_H
#define ATTRIBS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__
#define ATTRIBUTE(attrs) __attribute__(attrs)
#else
#define ATTRIBUTE(attrs)
#endif

#define ATTR_NORETURN ATTRIBUTE((noreturn))
#define ATTR_UNUSED   ATTRIBUTE((unused))
#define ATTR_MALLOC   ATTRIBUTE((malloc))

#define ATTR_PRINTF(idx_fmt, idx_args) ATTRIBUTE((format(printf, idx_fmt, idx_args)))

#define ATTR_NONNULL(arg_idx, ...) ATTRIBUTE((nonnull(arg_idx, ##__VA_ARGS__)))

#ifdef __cplusplus
}
#endif

#endif // ATTRIBS_H
