/**
 * @file dj_time_marker.c
 * @brief Markers for dj can be used to measure the time taken by different parts of the code
 * @author Cyprien Ménard
 * @date 12/2024
 * @see dj_time_marker.h
 *
 * @copyright Cecill-C (Cf. LICENCE.txt)
 */

/* ******************************************************* Includes ****************************************************** */

#include "utils/dj/dj_logs/dj_time_marker.h"
#include "system/log/log.h"
#include "utils/macros/macros.h"
#include "utils/time/time.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

LOG_REGISTER("utils/dj/time_marker");

/* **************************************************** Private macros *************************************************** */

/*
 * Metrics printed by dj_print_session_durations() / dj_print_all_durations():
 *
 *  SELF(us)   -- Self time of the marker on the last call, i.e. the duration not accounted for
 *                by any instrumented child. SELF = LAST - sum of direct children durations.
 *                A high SELF value indicates work not covered by a child marker.
 *
 *  LAST(us)   -- Raw duration of the last call (start -> end), including children.
 *                Useful to contextualise %PAR and compare with previous calls.
 *
 *  TOTAL(us)  -- Cumulative sum of all raw durations since the last reset.
 *                Identifies the most expensive markers over time, even when individual calls
 *                are fast (e.g. markers invoked hundreds of times in a loop).
 *
 *  CALLS      -- Number of completed start->end cycles since the last reset.
 *                A high count reveals an inner loop; multiply by AVG to verify TOTAL.
 *
 *  AVG(us)    -- Mean cost per invocation: TOTAL / CALLS.
 *
 *  MAX(us)    -- Longest single call recorded since the last reset.
 *                A MAX much larger than AVG indicates high variance (cache miss, lock, etc.).
 *
 *  %PAR       -- Ratio LAST / LAST(parent) * 100 for the last call.
 *                Shows what fraction this marker represents within its direct parent.
 *                '---' for root markers (no parent).
 *                '?%'  for markers called from more than one distinct parent (*).
 *
 * Marker prefixed with '*': called from at least two different parents across all calls.
 *                           Statistics are aggregated across all call contexts.
 */

/** @brief Bit position in parent_mask for markers called at the root level (no active parent) */
#define DJ_MARKER_ROOT_BIT   ((uint32_t)1 << (uint32_t)DJ_TIME_MARKER_COUNT)

/**
 * @brief Number of markers that can be active simultaneously (stack size)
 */
#define DJ_MARKER_STACK_SIZE (16)

/* ************************************************ Private type definition ********************************************** */

typedef struct
{
    time_us_t start_time;     /**< Timestamp of the active chronometer start [us] */
    time_us_t last_duration;  /**< Duration of the last completed start->end cycle [us] */
    time_us_t total_duration; /**< Cumulative duration since the last full reset [us] */
    time_us_t min_duration;   /**< Minimum single-call duration (valid when call_count > 0) [us] */
    time_us_t max_duration;   /**< Maximum single-call duration [us] */
    time_us_t
        current_children_duration; /**< Children duration accumulator for the in-progress call [us] */
    time_us_t last_children_duration;  /**< Children duration of the last completed call [us] */
    time_us_t children_total_duration; /**< Cumulative children duration over all calls [us] */
    uint32_t call_count;               /**< Number of completed start->end cycles */
    uint32_t
        parent_mask; /**< Bitmask of all parent markers seen (bit N = marker N is a parent); DJ_MARKER_ROOT_BIT = root level */
    bool used_in_session; /**< Set to true on start, cleared by dj_mark_session_reset */
} dj_time_marker_data_t;

/** @brief Active marker call stack used to detect nesting and track parent-child relationships */
typedef struct
{
    dj_time_marker_t markers[DJ_MARKER_STACK_SIZE]; /**< Stack of currently active marker types */
    uint8_t depth;                                  /**< Current stack depth */
} dj_marker_stack_t;

/* ********************************************* Private functions declarations ****************************************** */

static void print_marker_tree(dj_time_marker_t type, uint8_t depth, time_us_t parent_last_duration);
static void print_tree(dj_time_marker_t parent_id, uint8_t depth, time_us_t parent_last_duration);

/* ************************************************** Private variables ************************************************** */

/**
 * @brief Human-readable names matching each DJ_MARKERS entry
 */
static const char *const dj_time_marker_names[] = {FOREACH(WORD_CONVERT_TO_STRING, DJ_MARKERS)};

/**
 * @brief Statistics for each time marker
 */
static dj_time_marker_data_t dj_time_markers[DJ_TIME_MARKER_COUNT];

/**
 * @brief Active marker call stack for nesting detection
 */
static dj_marker_stack_t g_marker_stack;

/* ********************************************** Private functions definitions ****************************************** */

static void print_marker_tree(dj_time_marker_t type, uint8_t depth, time_us_t parent_last_duration)
{
    const dj_time_marker_data_t *m = &dj_time_markers[type];
    int avg_us = (m->call_count > 0) ? (int)(m->total_duration / (time_us_t)m->call_count) : 0;
    int self_us = (m->last_children_duration <= m->last_duration)
                      ? (int)(m->last_duration - m->last_children_duration)
                      : 0;

    // Detect multi-parent: more than one bit set in parent_mask
    bool multi_parent = (m->parent_mask != 0u) && ((m->parent_mask & (m->parent_mask - 1u)) != 0u);

    // Build indented name with a prefix indicator for ' '
    char name_buf[64];
    int pos = 0;
    for (uint8_t i = 0u; i < depth && pos < (int)(sizeof(name_buf) - 2); i++)
    {
        name_buf[pos++] = ' ';
    }
    if (pos < (int)(sizeof(name_buf) - 1))
    {
        name_buf[pos++] = multi_parent ? '*' : ' ';
    }
    (void)
        snprintf(name_buf + pos, sizeof(name_buf) - (size_t)pos, "%s", dj_time_marker_names[type]);

    // Compute %PAR column
    char par_buf[8];
    if (depth == 0u)
    {
        (void)snprintf(par_buf, sizeof(par_buf), "  ---");
    }
    else if (multi_parent || parent_last_duration == 0u)
    {
        (void)snprintf(par_buf, sizeof(par_buf), "   ?%%");
    }
    else
    {
        int pct = (int)((m->last_duration * 100u) / parent_last_duration);
        (void)snprintf(par_buf, sizeof(par_buf), "%4d%%", pct);
    }

    LOGD("  %-42s %7d %7d %10d %5u %7d %7d %5s",
         name_buf,
         self_us,
         (int)m->last_duration,
         (int)m->total_duration,
         m->call_count,
         avg_us,
         (int)m->max_duration,
         par_buf);
}

static void print_tree(dj_time_marker_t parent_id, uint8_t depth, time_us_t parent_last_duration)
{
    if (depth >= DJ_MARKER_STACK_SIZE)
    {
        return;
    }

    uint32_t parent_bit =
        (parent_id == DJ_TIME_MARKER_COUNT) ? DJ_MARKER_ROOT_BIT : (1u << (uint32_t)parent_id);

    for (dj_time_marker_t i = 0; i < DJ_TIME_MARKER_COUNT; i++)
    {
        if (!dj_time_markers[i].used_in_session)
        {
            continue;
        }
        if (!(dj_time_markers[i].parent_mask & parent_bit))
        {
            continue;
        }
        print_marker_tree(i, depth, parent_last_duration);
        print_tree(i, (uint8_t)(depth + 1u), dj_time_markers[i].last_duration);
    }
}

/* ********************************************** Public functions definitions ******************************************* */

void dj_mark_reset(dj_time_marker_t type)
{
    dj_time_markers[type].start_time = 0;
    dj_time_markers[type].last_duration = 0;
    dj_time_markers[type].total_duration = 0;
    dj_time_markers[type].min_duration = 0;
    dj_time_markers[type].max_duration = 0;
    dj_time_markers[type].current_children_duration = 0;
    dj_time_markers[type].last_children_duration = 0;
    dj_time_markers[type].children_total_duration = 0;
    dj_time_markers[type].call_count = 0;
    dj_time_markers[type].parent_mask = 0;
    dj_time_markers[type].used_in_session = false;
}

void dj_mark_reset_all(void)
{
    for (dj_time_marker_t i = 0; i < DJ_TIME_MARKER_COUNT; i++)
    {
        dj_mark_reset(i);
    }
    g_marker_stack.depth = 0;
}

void dj_mark_session_reset(void)
{
    for (dj_time_marker_t i = 0; i < DJ_TIME_MARKER_COUNT; i++)
    {
        dj_time_markers[i].used_in_session = false;
        dj_time_markers[i].parent_mask = 0;
        dj_time_markers[i].current_children_duration = 0;
    }
    g_marker_stack.depth = 0;
}

void dj_mark_start_time(dj_time_marker_t type)
{
    // Determine parent from the active call stack
    dj_time_marker_t parent = (g_marker_stack.depth > 0u)
                                  ? g_marker_stack.markers[g_marker_stack.depth - 1u]
                                  : DJ_TIME_MARKER_COUNT;

    // Record parent context in the bitmask (tracks all parents seen across all calls)
    uint32_t parent_bit =
        (parent == DJ_TIME_MARKER_COUNT) ? DJ_MARKER_ROOT_BIT : (1u << (uint32_t)parent);
    dj_time_markers[type].parent_mask |= parent_bit;

    // Reset children accumulator for this new call
    dj_time_markers[type].current_children_duration = 0;

    // Push onto the stack (guard against overflow)
    if (g_marker_stack.depth < DJ_MARKER_STACK_SIZE)
    {
        g_marker_stack.markers[g_marker_stack.depth] = type;
        g_marker_stack.depth++;
    }

    // Mark as used in session and record start time last (most accurate)
    dj_time_markers[type].used_in_session = true;
    dj_time_markers[type].start_time = time_get_absolute_time_us();
}

void dj_mark_end_time(dj_time_marker_t type)
{
    time_us_t duration = time_get_absolute_time_us() - dj_time_markers[type].start_time;
    dj_time_markers[type].last_duration = duration;
    dj_time_markers[type].total_duration += duration;
    if (dj_time_markers[type].call_count == 0u || duration < dj_time_markers[type].min_duration)
    {
        dj_time_markers[type].min_duration = duration;
    }
    if (duration > dj_time_markers[type].max_duration)
    {
        dj_time_markers[type].max_duration = duration;
    }
    dj_time_markers[type].call_count++;

    // Finalize children duration tracking for this call
    dj_time_markers[type].last_children_duration = dj_time_markers[type].current_children_duration;
    dj_time_markers[type].children_total_duration +=
        dj_time_markers[type].current_children_duration;

    // Pop from the call stack
    if (g_marker_stack.depth > 0u)
    {
        g_marker_stack.depth--;
    }

    // Contribute this marker's full duration to the parent's children accumulator
    if (g_marker_stack.depth > 0u)
    {
        dj_time_marker_t parent = g_marker_stack.markers[g_marker_stack.depth - 1u];
        dj_time_markers[parent].current_children_duration += duration;
    }
}

void dj_print_duration(dj_time_marker_t type)
{
    LOGD("  %-42s %7s %7s %10s %5s %7s %7s %5s",
         "MARKER",
         "SELF(us)",
         "LAST(us)",
         "TOTAL(us)",
         "CALLS",
         "AVG(us)",
         "MAX(us)",
         "%PAR");
    print_marker_tree(type, 0u, 0u);
}

void dj_print_all_durations(void)
{
    LOGD("=== DJ [all markers] ===");
    LOGD("  %-42s %7s %7s %10s %5s %7s %7s %5s",
         "MARKER",
         "SELF(us)",
         "LAST(us)",
         "TOTAL(us)",
         "CALLS",
         "AVG(us)",
         "MAX(us)",
         "%PAR");
    for (dj_time_marker_t i = 0; i < DJ_TIME_MARKER_COUNT; i++)
    {
        print_marker_tree(i, 0u, 0u);
    }
}

void dj_print_session_durations(const char *operation_name)
{
    LOGD("=== DJ [%s] ===", operation_name);
    LOGD("  %-42s %7s %7s %10s %5s %7s %7s %5s",
         "MARKER",
         "SELF(us)",
         "LAST(us)",
         "TOTAL(us)",
         "CALLS",
         "AVG(us)",
         "MAX(us)",
         "%PAR");
    print_tree(DJ_TIME_MARKER_COUNT, 0u, 0u);
}

/* ***************************************** Public callback functions definitions *************************************** */
