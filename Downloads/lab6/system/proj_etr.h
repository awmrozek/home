;!**************************************************************************
;!
;!  FILE NAME:     proj_etr.h
;!
;!  DESCRIPTION:   Project specific definitions for OSYS
;!
;!
;!---------------------------------------------------------------------------
;!  HISTORY
;!
;!  DATE          NAME            CHANGES
;!  ----          ----            -------
;!  Sep 05 1995   Jens Johansson  First version for cdrom server.
;!
;! ---------------------------------------------------------------------------
;!
;! (C) 1995 Axis Communications AB, Lund, Sweden
;!
;!**************************************************************************
; @(#) proj_etr.h 1.1 10/17/95 

;**************************  CONSTANTS  ************************************
#ifdef PRUNED_CODE

# ifdef MEDIA_TOKEN_RING
.set NOF_TASKS,  5          ; Number of existing tasks
                            ; This must be the same as in projos.h
# else
.set NOF_TASKS,  4          ; Number of existing tasks
                            ; This must be the same as in projos.h
# endif /* MEDIA_TOKEN_RING */
#else  /* ?PRUNED_CODE */

# ifdef MEDIA_TOKEN_RING
.set NOF_TASKS,  8          ; Number of existing tasks
                            ; This must be the same as in projos.h
# else
.set NOF_TASKS,  7          ; Number of existing tasks
                            ; This must be the same as in projos.h
# endif /* MEDIA_TOKEN_RING */

#endif /* PRUNED_CODE */

;**************************  EXTERNALS *************************************

;**************************  EXTERNAL FUNCTIONS ****************************

;**************************  End of proj_etr.h *****************************

