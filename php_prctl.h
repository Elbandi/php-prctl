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

#ifndef PHP_PRCTL_H
#define PHP_PRCTL_H

#define PHP_PRCTL_VERSION "0.1"

extern zend_module_entry prctl_module_entry;
#define phpext_prctl_ptr &prctl_module_entry

#ifdef PHP_WIN32
#	define PHP_PRCTL_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_PRCTL_API __attribute__ ((visibility("default")))
#else
#	define PHP_PRCTL_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(prctl);
PHP_MSHUTDOWN_FUNCTION(prctl);
PHP_RINIT_FUNCTION(prctl);
PHP_RSHUTDOWN_FUNCTION(prctl);
PHP_MINFO_FUNCTION(prctl);

PHP_FUNCTION(confirm_prctl_compiled);	/* For testing, remove later. */
PHP_FUNCTION(prctl_capbset_read);
PHP_FUNCTION(prctl_capbset_drop);
PHP_FUNCTION(prctl_set_dumpable);
PHP_FUNCTION(prctl_get_dumpable);
PHP_FUNCTION(prctl_set_endian);
PHP_FUNCTION(prctl_get_endian);
PHP_FUNCTION(prctl_set_fpemu);
PHP_FUNCTION(prctl_get_fpemu);
PHP_FUNCTION(prctl_set_fpexc);
PHP_FUNCTION(prctl_get_fpexc);
PHP_FUNCTION(prctl_set_keepcaps);
PHP_FUNCTION(prctl_get_keepcaps);
PHP_FUNCTION(prctl_set_name);
PHP_FUNCTION(prctl_get_name);
PHP_FUNCTION(prctl_set_pdeathsig);
PHP_FUNCTION(prctl_get_pdeathsig);
PHP_FUNCTION(prctl_set_seccomp);
PHP_FUNCTION(prctl_get_seccomp);
PHP_FUNCTION(prctl_set_securebits);
PHP_FUNCTION(prctl_get_securebits);
PHP_FUNCTION(prctl_set_tsc);
PHP_FUNCTION(prctl_get_tsc);
PHP_FUNCTION(prctl_set_unalign);
PHP_FUNCTION(prctl_get_unalign);
#ifdef PR_MCE_KILL
PHP_FUNCTION(prctl_set_mce_kill);
PHP_FUNCTION(prctl_set_mce_clear);
PHP_FUNCTION(prctl_get_mce_kill);
#endif
#ifdef PR_SET_TIMERSLACK
PHP_FUNCTION(prctl_set_timerslack);
PHP_FUNCTION(prctl_get_timerslack);
#endif
#ifdef PR_TASK_PERF_EVENTS_DISABLE
PHP_FUNCTION(prctl_task_perf_events_disable);
PHP_FUNCTION(prctl_task_perf_events_enable);
#endif
#ifdef PR_SET_MM
PHP_FUNCTION(prctl_set_mm);
#endif
#ifdef PR_SET_PTRACER
PHP_FUNCTION(prctl_set_ptracer);
#endif
#ifdef PR_SET_CHILD_SUBREAPER
PHP_FUNCTION(prctl_set_child_subreaper);
PHP_FUNCTION(prctl_get_child_subreaper);
#endif
#ifdef PR_SET_NO_NEW_PRIVS
PHP_FUNCTION(prctl_set_no_new_privs);
PHP_FUNCTION(prctl_get_no_new_privs);
PHP_FUNCTION(prctl_get_tid_address);
#endif
#ifdef PR_SET_THP_DISABLE
PHP_FUNCTION(prctl_set_thp_disable);
PHP_FUNCTION(prctl_get_thp_disable);
#endif
#ifdef PR_SET_FP_MODE
PHP_FUNCTION(prctl_set_fp_mode);
PHP_FUNCTION(prctl_get_fp_mode);
#endif

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(prctl)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(prctl)
*/

/* In every utility function you add that needs to use variables 
   in php_prctl_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as PRCTL_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define PRCTL_G(v) TSRMG(prctl_globals_id, zend_prctl_globals *, v)
#else
#define PRCTL_G(v) (prctl_globals.v)
#endif

#endif	/* PHP_PRCTL_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
