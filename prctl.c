/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2008 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id: header 252479 2008-02-07 19:39:50Z iliaa $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_prctl.h"
#include "securebits.h"
#include <sys/capability.h>
#include <sys/prctl.h>
#include <sys/signal.h>

#define PRCTL_CONST(tag) REGISTER_LONG_CONSTANT("PRCTL_" #tag, PR_##tag, CONST_CS | CONST_PERSISTENT)

/* If you declare any globals in php_prctl.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(prctl)
*/

/* True global resources - no need for thread safety here */
static int le_prctl;

/* {{{ prctl_functions[]
 *
 * Every user visible function must have an entry in prctl_functions[].
 */
const zend_function_entry prctl_functions[] = {
	PHP_FE(confirm_prctl_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(prctl_capbset_read,	NULL)
	PHP_FE(prctl_capbset_drop,	NULL)
	PHP_FE(prctl_set_dumpable,	NULL)
	PHP_FE(prctl_get_dumpable,	NULL)
	PHP_FE(prctl_set_endian,	NULL)
	PHP_FE(prctl_get_endian,	NULL)
	PHP_FE(prctl_set_fpemu,	NULL)
	PHP_FE(prctl_get_fpemu,	NULL)
	PHP_FE(prctl_set_fpexc,	NULL)
	PHP_FE(prctl_get_fpexc,	NULL)
	PHP_FE(prctl_set_keepcaps,	NULL)
	PHP_FE(prctl_get_keepcaps,	NULL)
	PHP_FE(prctl_set_name,	NULL)
	PHP_FE(prctl_get_name,	NULL)
	PHP_FE(prctl_set_pdeathsig,	NULL)
	PHP_FE(prctl_get_pdeathsig,	NULL)
	PHP_FE(prctl_set_seccomp,	NULL)
	PHP_FE(prctl_get_seccomp,	NULL)
	PHP_FE(prctl_set_securebits,	NULL)
	PHP_FE(prctl_get_securebits,	NULL)
	PHP_FE(prctl_set_tsc,	NULL)
	PHP_FE(prctl_get_tsc,	NULL)
	PHP_FE(prctl_set_unalign,	NULL)
	PHP_FE(prctl_get_unalign,	NULL)
#ifdef PR_MCE_KILL
	PHP_FE(prctl_set_mce_kill,	NULL)
	PHP_FE(prctl_set_mce_clear,	NULL)
	PHP_FE(prctl_get_mce_kill,	NULL)
#endif
#ifdef PR_SET_TIMERSLACK
	PHP_FE(prctl_set_timerslack,	NULL)
	PHP_FE(prctl_get_timerslack,	NULL)
#endif
#ifdef PR_TASK_PERF_EVENTS_DISABLE
	PHP_FE(prctl_task_perf_events_disable,	NULL)
	PHP_FE(prctl_task_perf_events_enable,	NULL)
#endif
#ifdef PR_SET_PTRACER
	PHP_FE(prctl_set_ptracer,	NULL)
#endif
#ifdef PR_SET_CHILD_SUBREAPER
	PHP_FE(prctl_set_child_subreaper,	NULL)
	PHP_FE(prctl_get_child_subreaper,	NULL)
#endif
#ifdef PR_SET_NO_NEW_PRIVS
	PHP_FE(prctl_set_no_new_privs,	NULL)
	PHP_FE(prctl_get_no_new_privs,	NULL)
	PHP_FE(prctl_get_tid_address,	NULL)
#endif
#ifdef PR_SET_THP_DISABLE
	PHP_FE(prctl_set_thp_disable,	NULL)
	PHP_FE(prctl_get_thp_disable,	NULL)
#endif
#ifdef PR_SET_FP_MODE
	PHP_FE(prctl_set_fp_mode,	NULL)
	PHP_FE(prctl_get_fp_mode,	NULL)
#endif

	{NULL, NULL, NULL}	/* Must be the last line in prctl_functions[] */
};
/* }}} */

/* {{{ prctl_module_entry
 */
zend_module_entry prctl_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"prctl",
	prctl_functions,
	PHP_MINIT(prctl),
	PHP_MSHUTDOWN(prctl),
	PHP_RINIT(prctl),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(prctl),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(prctl),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_PRCTL_VERSION, /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PRCTL
ZEND_GET_MODULE(prctl)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("prctl.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_prctl_globals, prctl_globals)
    STD_PHP_INI_ENTRY("prctl.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_prctl_globals, prctl_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_prctl_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_prctl_init_globals(zend_prctl_globals *prctl_globals)
{
	prctl_globals->global_value = 0;
	prctl_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(prctl)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	PRCTL_CONST(ENDIAN_LITTLE);
	PRCTL_CONST(ENDIAN_BIG);
	PRCTL_CONST(ENDIAN_PPC_LITTLE);

	PRCTL_CONST(FPEMU_NOPRINT);
	PRCTL_CONST(FPEMU_SIGFPE);

	PRCTL_CONST(FP_EXC_SW_ENABLE);
	PRCTL_CONST(FP_EXC_DIV);
	PRCTL_CONST(FP_EXC_OVF);
	PRCTL_CONST(FP_EXC_UND);
	PRCTL_CONST(FP_EXC_RES);
	PRCTL_CONST(FP_EXC_INV);
	PRCTL_CONST(FP_EXC_DISABLED);
	PRCTL_CONST(FP_EXC_NONRECOV);
	PRCTL_CONST(FP_EXC_ASYNC);
	PRCTL_CONST(FP_EXC_PRECISE);

	PRCTL_CONST(TIMING_STATISTICAL);
	PRCTL_CONST(TIMING_TIMESTAMP);

	PRCTL_CONST(TSC_ENABLE);
	PRCTL_CONST(TSC_SIGSEGV);

	PRCTL_CONST(UNALIGN_NOPRINT);
	PRCTL_CONST(UNALIGN_SIGBUS);

	PRCTL_CONST(MCE_KILL_DEFAULT);
	PRCTL_CONST(MCE_KILL_EARLY);
	PRCTL_CONST(MCE_KILL_LATE);

#ifdef PR_SET_PTRACER
	PRCTL_CONST(SET_PTRACER_ANY);
#endif

#ifdef PR_SET_FP_MODE
	PRCTL_CONST(FP_MODE_FR);
	PRCTL_CONST(FP_MODE_FRE);
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(prctl)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(prctl)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(prctl)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(prctl)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "prctl support", "enabled");
	php_info_print_table_header(2, "version", PHP_PRCTL_VERSION);
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_prctl_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_prctl_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "prctl", arg);
	RETURN_STRINGL(strg, len, 0);
}
/* }}} */

/* Code get from: https://github.com/seveas/python-prctl */

/* New in 2.6.32, but named and implemented inconsistently. The linux
 * implementation has two ways of setting the policy to the default, and thus
 * needs an extra argument. We ignore the first argument and always all
 * PR_MCE_KILL_SET. This makes our implementation simpler and keeps the prctl
 * interface more consistent
 */
#ifdef PR_MCE_KILL
#define PR_GET_MCE_KILL PR_MCE_KILL_GET
#define PR_SET_MCE_KILL PR_MCE_KILL
#endif

/* New in 2.6.XX (Ubuntu 10.10) */
#define NOT_SET (-1)
#ifdef PR_SET_PTRACER
/* This one has no getter for some reason, but guard agains that being fixed  */
#ifndef PR_GET_PTRACER
#define PR_GET_PTRACER NOT_SET
/* Icky global variable to cache ptracer */
static int __cached_ptracer = NOT_SET;
#endif
#endif

/* {{{ prctl_prctl */
static void prctl_prctl(INTERNAL_FUNCTION_PARAMETERS, int option) /* {{{ */
{
	long arg = 0;
	char *argstr = NULL;
	int argstr_len = 0;
	char name[17] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
	int result;
	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "|l", &arg) == FAILURE) {
		if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s", &argstr, &argstr_len) == FAILURE) {
			return;
		}
		if(option != PR_SET_NAME) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "a integer is required");
			RETURN_FALSE;
		}
	} else {
		if(option == PR_SET_NAME) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "a string is required");
			RETURN_FALSE;
		}
	}

	switch (option) {
		case (PR_CAPBSET_READ):
		case (PR_CAPBSET_DROP):
			if (!cap_valid(arg)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown capability");
				RETURN_FALSE;
			}
			break;
		case (PR_SET_DUMPABLE):
		case (PR_SET_KEEPCAPS):
			arg = arg ? 1 : 0;
			break;
		case (PR_SET_ENDIAN):
			if(arg != PR_ENDIAN_LITTLE && arg != PR_ENDIAN_BIG && arg != PR_ENDIAN_PPC_LITTLE) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown endianness");
				RETURN_FALSE;
			}
			break;
		case (PR_SET_FPEMU):
			if(arg != PR_FPEMU_NOPRINT && arg != PR_FPEMU_SIGFPE) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown floating-point emulation setting");
				RETURN_FALSE;
			}
		case (PR_SET_FPEXC):
			if(arg & ~(PR_FP_EXC_SW_ENABLE | PR_FP_EXC_DIV | PR_FP_EXC_OVF | PR_FP_EXC_UND | PR_FP_EXC_RES |
				   PR_FP_EXC_INV | PR_FP_EXC_DISABLED | PR_FP_EXC_NONRECOV | PR_FP_EXC_ASYNC | PR_FP_EXC_PRECISE)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown floating-point exception mode");
				RETURN_FALSE;
			}
#if PR_MCE_KILL
		case (PR_SET_MCE_KILL):
			if(arg != PR_MCE_KILL_DEFAULT && arg != PR_MCE_KILL_EARLY && arg != PR_MCE_KILL_LATE) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown memory corruption kill policy");
				RETURN_FALSE;
			}
			break;
#endif
		case (PR_SET_NAME):
			if(strlen(argstr) > 16) {
				php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Name is truncated to 16 length");
			}
			strncpy(name, argstr, 16);
			break;
		case (PR_SET_PDEATHSIG):
			if(arg < 0 || arg > SIGRTMAX) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown signal");
				RETURN_FALSE;
			}
			break;
		case (PR_SET_SECCOMP):
#ifdef PR_TASK_PERF_EVENTS_DISABLE
		case (PR_TASK_PERF_EVENTS_DISABLE):
		case (PR_TASK_PERF_EVENTS_ENABLE):
#endif
#ifdef PR_SET_NO_NEW_PRIVS
		case(PR_SET_NO_NEW_PRIVS):
#endif
			if(!arg) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument must be 1");
				RETURN_FALSE;
			}
			arg = 1;
			break;
		case (PR_SET_SECUREBITS):
			if(arg & ~ ((1 << SECURE_NOROOT) | (1 << SECURE_NOROOT_LOCKED) | (1 << SECURE_NO_SETUID_FIXUP) |
			            (1 << SECURE_NO_SETUID_FIXUP_LOCKED) | (1 << SECURE_KEEP_CAPS) | (1 << SECURE_KEEP_CAPS_LOCKED))) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid securebits set");
				RETURN_FALSE;
			}
			break;
		case (PR_SET_TIMING):
			if(arg != PR_TIMING_STATISTICAL && arg != PR_TIMING_TIMESTAMP) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid timing constant");
				RETURN_FALSE;
			}
			break;
		case (PR_SET_TSC):
			if(arg != PR_TSC_ENABLE && arg != PR_TSC_SIGSEGV) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid TSC setting");
				RETURN_FALSE;
			}
			break;
		case (PR_SET_UNALIGN):
			if(arg != PR_UNALIGN_NOPRINT && arg != PR_UNALIGN_SIGBUS) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid UNALIGN setting");
				RETURN_FALSE;
			}
			break;
#ifdef PR_SET_FP_MODE
		case (PR_SET_FP_MODE):
			if(arg != PR_FP_MODE_FR && arg != PR_FP_MODE_FRE) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid FP_MODE setting");
				RETURN_FALSE;
			}
			break;
#endif
	}

	/*
	 * Calling prctl 
	 * There are 3 basic call modes:
	 * - Setters and getters for which the return value is the result
	 * - Getters for which the result is placed in arg2
	 * - Getters and setters that deal with strings.
	 *
	 * This function takes care of all that and always returns Py_None for
	 * settings or the result of a getter call as a PyInt or PyString.
	 */
	switch(option) {
		case(PR_CAPBSET_READ):
		case(PR_CAPBSET_DROP):
#ifdef PR_SET_CHILD_SUBREAPER
		case(PR_SET_CHILD_SUBREAPER):
#endif
		case(PR_SET_DUMPABLE):
		case(PR_GET_DUMPABLE):
		case(PR_SET_ENDIAN):
		case(PR_SET_FPEMU):
		case(PR_SET_FPEXC):
		case(PR_SET_KEEPCAPS):
		case(PR_GET_KEEPCAPS):
#ifdef PR_MCE_KILL
		case(PR_GET_MCE_KILL):
#endif
#ifdef PR_GET_NO_NEW_PRIVS
		case(PR_GET_NO_NEW_PRIVS):
		case(PR_SET_NO_NEW_PRIVS):
#endif
#ifdef PR_TASK_PERF_EVENTS_DISABLE
		case(PR_TASK_PERF_EVENTS_DISABLE):
		case(PR_TASK_PERF_EVENTS_ENABLE):
#endif
		case(PR_SET_PDEATHSIG):
#if defined(PR_GET_PTRACER) && (PR_GET_PTRACER != NOT_SET)
		case(PR_GET_PTRACER):
#endif
#ifdef PR_SET_PTRACER
		case(PR_SET_PTRACER):
#endif
		case(PR_SET_SECCOMP):
		case(PR_GET_SECCOMP):
		case(PR_SET_SECUREBITS):
		case(PR_GET_SECUREBITS):
#ifdef PR_GET_TIMERSLACK
		case(PR_GET_TIMERSLACK):
		case(PR_SET_TIMERSLACK):
#endif
		case(PR_SET_TIMING):
		case(PR_GET_TIMING):
		case(PR_SET_TSC):
		case(PR_SET_UNALIGN):
#ifdef PR_SET_THP_DISABLE
		case(PR_SET_THP_DISABLE):
		case(PR_GET_THP_DISABLE):
#endif
#ifdef PR_SET_FP_MODE
		case(PR_SET_FP_MODE):
#endif
			result = prctl(option, arg, 0, 0, 0);
			if(result < 0) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", strerror(errno));
				RETURN_FALSE;
			}
			switch(option) {
				case(PR_CAPBSET_READ):
				case(PR_GET_DUMPABLE):
				case(PR_GET_KEEPCAPS):
				case(PR_GET_SECCOMP):
				case(PR_GET_TIMING):
					RETURN_LONG(result);
#ifdef PR_MCE_KILL
				case(PR_GET_MCE_KILL):
#endif
#ifdef PR_GET_NO_NEW_PRIVS
				case(PR_GET_NO_NEW_PRIVS):
#endif
#if defined(PR_GET_PTRACER) && (PR_GET_PTRACER != NOT_SET)
				case(PR_GET_PTRACER):
#endif
				case(PR_GET_SECUREBITS):
#ifdef PR_GET_TIMERSLACK
				case(PR_GET_TIMERSLACK):
#endif
#ifdef PR_SET_THP_DISABLE
				case(PR_GET_THP_DISABLE):
#endif
					RETURN_LONG(result);
#if defined(PR_GET_PTRACER) && (PR_GET_PTRACER == NOT_SET)
				case(PR_SET_PTRACER):
					__cached_ptracer = arg;
					break;
#endif
			}
			break;
#ifdef PR_GET_CHILD_SUBREAPER
		case(PR_GET_CHILD_SUBREAPER):
#endif
		case(PR_GET_ENDIAN):
		case(PR_GET_FPEMU):
		case(PR_GET_FPEXC):
		case(PR_GET_PDEATHSIG):
		case(PR_GET_TSC):
		case(PR_GET_UNALIGN):
#ifdef PR_GET_TID_ADDRESS
		case(PR_GET_TID_ADDRESS):
#endif
			result = prctl(option, &arg, 0, 0, 0);
			if(result < 0) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", strerror(errno));
				RETURN_FALSE;
			}
			RETURN_LONG(arg);
		case(PR_SET_NAME):
		case(PR_GET_NAME):
			result = prctl(option, name, 0, 0, 0);
			if(result < 0) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", strerror(errno));
				RETURN_FALSE;
			}
			if(option == PR_GET_NAME) {
				RETURN_STRING(name, 1);
			}
			break;
#if defined(PR_GET_PTRACER) && (PR_GET_PTRACER == NOT_SET)
		case(PR_GET_PTRACER):
			if(__cached_ptracer == NOT_SET)
				RETURN_LONG(getppid());
			RETURN_LONG(__cached_ptracer);
#endif
#ifdef PR_MCE_KILL
		case(PR_MCE_KILL_CLEAR):
			result = prctl(option, PR_MCE_KILL_CLEAR, 0, 0, 0);
			if(result < 0) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", strerror(errno));
				RETURN_FALSE;
			}
			break;
		case(PR_SET_MCE_KILL):
			result = prctl(option, PR_MCE_KILL_SET, arg, 0, 0);
			if(result < 0) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", strerror(errno));
				RETURN_FALSE;
			}
			break;
#endif
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING,"Unkown prctl option");
			RETURN_FALSE;
	}

	/* None is returned by default */
	RETURN_TRUE;
}
/* }}} */

/* {{{ prctl_functions */
#define PRCTL_FUNCTION(x,y) PHP_FUNCTION(x) \
{ \
	prctl_prctl(INTERNAL_FUNCTION_PARAM_PASSTHRU, y); \
}

PRCTL_FUNCTION(prctl_capbset_read, PR_CAPBSET_READ);
PRCTL_FUNCTION(prctl_capbset_drop, PR_CAPBSET_DROP);
PRCTL_FUNCTION(prctl_set_dumpable, PR_SET_DUMPABLE);
PRCTL_FUNCTION(prctl_get_dumpable, PR_GET_DUMPABLE);
PRCTL_FUNCTION(prctl_set_endian, PR_SET_ENDIAN);
PRCTL_FUNCTION(prctl_get_endian, PR_GET_ENDIAN);
PRCTL_FUNCTION(prctl_set_fpemu, PR_SET_FPEMU);
PRCTL_FUNCTION(prctl_get_fpemu, PR_GET_FPEMU);
PRCTL_FUNCTION(prctl_set_fpexc, PR_SET_FPEXC);
PRCTL_FUNCTION(prctl_get_fpexc, PR_GET_FPEXC);
PRCTL_FUNCTION(prctl_set_keepcaps, PR_SET_KEEPCAPS);
PRCTL_FUNCTION(prctl_get_keepcaps, PR_GET_KEEPCAPS);
PRCTL_FUNCTION(prctl_set_name, PR_SET_NAME);
PRCTL_FUNCTION(prctl_get_name, PR_GET_NAME);
PRCTL_FUNCTION(prctl_set_pdeathsig, PR_SET_PDEATHSIG);
PRCTL_FUNCTION(prctl_get_pdeathsig, PR_GET_PDEATHSIG);
PRCTL_FUNCTION(prctl_set_seccomp, PR_SET_SECCOMP);
PRCTL_FUNCTION(prctl_get_seccomp, PR_GET_SECCOMP);
PRCTL_FUNCTION(prctl_set_securebits, PR_SET_SECUREBITS);
PRCTL_FUNCTION(prctl_get_securebits, PR_GET_SECUREBITS);
PRCTL_FUNCTION(prctl_set_tsc, PR_SET_TSC);
PRCTL_FUNCTION(prctl_get_tsc, PR_GET_TSC);
PRCTL_FUNCTION(prctl_set_unalign, PR_SET_UNALIGN);
PRCTL_FUNCTION(prctl_get_unalign, PR_GET_UNALIGN);
#ifdef PR_MCE_KILL
PRCTL_FUNCTION(prctl_set_mce_kill, PR_SET_MCE_KILL);
PRCTL_FUNCTION(prctl_set_mce_clear, PR_SET_MCE_CLEAR);
PRCTL_FUNCTION(prctl_get_mce_kill, PR_GET_MCE_KILL);
#endif
#ifdef PR_SET_TIMERSLACK
PRCTL_FUNCTION(prctl_set_timerslack, PR_SET_TIMERSLACK);
PRCTL_FUNCTION(prctl_get_timerslack, PR_GET_TIMERSLACK);
#endif
#ifdef PR_TASK_PERF_EVENTS_DISABLE
PRCTL_FUNCTION(prctl_task_perf_events_disable, PR_TASK_PERF_EVENTS_DISABLE);
PRCTL_FUNCTION(prctl_task_perf_events_enable, PR_TASK_PERF_EVENTS_ENABLE);
#endif
#ifdef PR_SET_PTRACER
PRCTL_FUNCTION(prctl_set_ptracer, PR_SET_PTRACER);
#endif
#ifdef PR_SET_CHILD_SUBREAPER
PRCTL_FUNCTION(prctl_set_child_subreaper, PR_SET_CHILD_SUBREAPER);
PRCTL_FUNCTION(prctl_get_child_subreaper, PR_GET_CHILD_SUBREAPER);
#endif
#ifdef PR_SET_NO_NEW_PRIVS
PRCTL_FUNCTION(prctl_set_no_new_privs, PR_SET_NO_NEW_PRIVS);
PRCTL_FUNCTION(prctl_get_no_new_privs, PR_GET_NO_NEW_PRIVS);
PRCTL_FUNCTION(prctl_get_tid_address, PR_GET_TID_ADDRESS);
#endif
#ifdef PR_SET_THP_DISABLE
PRCTL_FUNCTION(prctl_set_thp_disable, PR_SET_THP_DISABLE);
PRCTL_FUNCTION(prctl_get_thp_disable, PR_GET_THP_DISABLE);
#endif
#ifdef PR_SET_FP_MODE
PRCTL_FUNCTION(prctl_set_fp_mode, PR_SET_FP_MODE);
PRCTL_FUNCTION(prctl_get_fp_mode, PR_GET_FP_MODE);
#endif

/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
