
/* errno.h library for large systems - small embedded systems use clibrary.c instead */
/* now, with the tiny ram patch we can use this. clibrary.c is used only when you disable
   optimizations (optram=0) */

#include <errno.h>
#include "interpreter.h"
#include "rotable.h"

#if ((PICOC_OPTIMIZE_MEMORY == 2) && !defined (BUILTIN_MINI_STDLIB))

#ifdef EACCES
static const int EACCESValue = EACCES;
#endif

#ifdef EADDRINUSE
static const int EADDRINUSEValue = EADDRINUSE;
#endif

#ifdef EADDRNOTAVAIL
static const int EADDRNOTAVAILValue = EADDRNOTAVAIL;
#endif

#ifdef EAFNOSUPPORT
static const int EAFNOSUPPORTValue = EAFNOSUPPORT;
#endif

#ifdef EAGAIN
static const int EAGAINValue = EAGAIN;
#endif

#ifdef EALREADY
static const int EALREADYValue = EALREADY;
#endif

#ifdef EBADF
static const int EBADFValue = EBADF;
#endif

#ifdef EBADMSG
static const int EBADMSGValue = EBADMSG;
#endif

#ifdef EBUSY
static const int EBUSYValue = EBUSY;
#endif

#ifdef ECANCELED
static const int ECANCELEDValue = ECANCELED;
#endif

#ifdef ECHILD
static const int ECHILDValue = ECHILD;
#endif

#ifdef ECONNABORTED
static const int ECONNABORTEDValue = ECONNABORTED;
#endif

#ifdef ECONNREFUSED
static const int ECONNREFUSEDValue = ECONNREFUSED;
#endif

#ifdef ECONNRESET
static const int ECONNRESETValue = ECONNRESET;
#endif

#ifdef EDEADLK
static const int EDEADLKValue = EDEADLK;
#endif

#ifdef EDESTADDRREQ
static const int EDESTADDRREQValue = EDESTADDRREQ;
#endif

#ifdef EDOM
static const int EDOMValue = EDOM;
#endif

#ifdef EDQUOT
static const int EDQUOTValue = EDQUOT;
#endif

#ifdef EEXIST
static const int EEXISTValue = EEXIST;
#endif

#ifdef EFAULT
static const int EFAULTValue = EFAULT;
#endif

#ifdef EFBIG
static const int EFBIGValue = EFBIG;
#endif

#ifdef EHOSTUNREACH
static const int EHOSTUNREACHValue = EHOSTUNREACH;
#endif

#ifdef EIDRM
static const int EIDRMValue = EIDRM;
#endif

#ifdef EILSEQ
static const int EILSEQValue = EILSEQ;
#endif

#ifdef EINPROGRESS
static const int EINPROGRESSValue = EINPROGRESS;
#endif

#ifdef EINTR
static const int EINTRValue = EINTR;
#endif

#ifdef EINVAL
static const int EINVALValue = EINVAL;
#endif

#ifdef EIO
static const int EIOValue = EIO;
#endif

#ifdef EISCONN
static const int EISCONNValue = EISCONN;
#endif

#ifdef EISDIR
static const int EISDIRValue = EISDIR;
#endif

#ifdef ELOOP
static const int ELOOPValue = ELOOP;
#endif

#ifdef EMFILE
static const int EMFILEValue = EMFILE;
#endif

#ifdef EMLINK
static const int EMLINKValue = EMLINK;
#endif

#ifdef EMSGSIZE
static const int EMSGSIZEValue = EMSGSIZE;
#endif

#ifdef EMULTIHOP
static const int EMULTIHOPValue = EMULTIHOP;
#endif

#ifdef ENAMETOOLONG
static const int ENAMETOOLONGValue = ENAMETOOLONG;
#endif

#ifdef ENETDOWN
static const int ENETDOWNValue = ENETDOWN;
#endif

#ifdef ENETRESET
static const int ENETRESETValue = ENETRESET;
#endif

#ifdef ENETUNREACH
static const int ENETUNREACHValue = ENETUNREACH;
#endif

#ifdef ENFILE
static const int ENFILEValue = ENFILE;
#endif

#ifdef ENOBUFS
static const int ENOBUFSValue = ENOBUFS;
#endif

#ifdef ENODATA
static const int ENODATAValue = ENODATA;
#endif

#ifdef ENODEV
static const int ENODEVValue = ENODEV;
#endif

#ifdef ENOENT
static const int ENOENTValue = ENOENT;
#endif

#ifdef ENOEXEC
static const int ENOEXECValue = ENOEXEC;
#endif

#ifdef ENOLCK
static const int ENOLCKValue = ENOLCK;
#endif

#ifdef ENOLINK
static const int ENOLINKValue = ENOLINK;
#endif

#ifdef ENOMEM
static const int ENOMEMValue = ENOMEM;
#endif

#ifdef ENOMSG
static const int ENOMSGValue = ENOMSG;
#endif

#ifdef ENOPROTOOPT
static const int ENOPROTOOPTValue = ENOPROTOOPT;
#endif

#ifdef ENOSPC
static const int ENOSPCValue = ENOSPC;
#endif

#ifdef ENOSR
static const int ENOSRValue = ENOSR;
#endif

#ifdef ENOSTR
static const int ENOSTRValue = ENOSTR;
#endif

#ifdef ENOSYS
static const int ENOSYSValue = ENOSYS;
#endif

#ifdef ENOTCONN
static const int ENOTCONNValue = ENOTCONN;
#endif

#ifdef ENOTDIR
static const int ENOTDIRValue = ENOTDIR;
#endif

#ifdef ENOTEMPTY
static const int ENOTEMPTYValue = ENOTEMPTY;
#endif

#ifdef ENOTRECOVERABLE
static const int ENOTRECOVERABLEValue = ENOTRECOVERABLE;
#endif

#ifdef ENOTSOCK
static const int ENOTSOCKValue = ENOTSOCK;
#endif

#ifdef ENOTSUP
static const int ENOTSUPValue = ENOTSUP;
#endif

#ifdef ENOTTY
static const int ENOTTYValue = ENOTTY;
#endif

#ifdef ENXIO
static const int ENXIOValue = ENXIO;
#endif

#ifdef EOPNOTSUPP
static const int EOPNOTSUPPValue = EOPNOTSUPP;
#endif

#ifdef EOVERFLOW
static const int EOVERFLOWValue = EOVERFLOW;
#endif

#ifdef EOWNERDEAD
static const int EOWNERDEADValue = EOWNERDEAD;
#endif

#ifdef EPERM
static const int EPERMValue = EPERM;
#endif

#ifdef EPIPE
static const int EPIPEValue = EPIPE;
#endif

#ifdef EPROTO
static const int EPROTOValue = EPROTO;
#endif

#ifdef EPROTONOSUPPORT
static const int EPROTONOSUPPORTValue = EPROTONOSUPPORT;
#endif

#ifdef EPROTOTYPE
static const int EPROTOTYPEValue = EPROTOTYPE;
#endif

#ifdef ERANGE
static const int ERANGEValue = ERANGE;
#endif

#ifdef EROFS
static const int EROFSValue = EROFS;
#endif

#ifdef ESPIPE
static const int ESPIPEValue = ESPIPE;
#endif

#ifdef ESRCH
static const int ESRCHValue = ESRCH;
#endif

#ifdef ESTALE
static const int ESTALEValue = ESTALE;
#endif

#ifdef ETIME
static const int ETIMEValue = ETIME;
#endif

#ifdef ETIMEDOUT
static const int ETIMEDOUTValue = ETIMEDOUT;
#endif

#ifdef ETXTBSY
static const int ETXTBSYValue = ETXTBSY;
#endif

#ifdef EWOULDBLOCK
static const int EWOULDBLOCKValue = EWOULDBLOCK;
#endif

#ifdef EXDEV
static const int EXDEVValue = EXDEV;
#endif

/* errno setup function */
void StdErrnoSetupFunc(void)
{
	/* since this variable is writable, we don't include this in the rotable */
	VariableDefinePlatformVar(NULL, "errno", &IntType, (union AnyValue *)&errno, TRUE);
}

#define MIN_OPT_LEVEL 2
#include "rodefs.h"

/* creates various system-dependent definitions */
const PICOC_RO_TYPE errno_variables[] = {
    /* defines */
#ifdef EACCES
	{STRKEY("EACCES"), INT(EACCESValue)},
#endif

#ifdef EADDRINUSE
	{STRKEY("EADDRINUSE"), INT(EADDRINUSEValue)},
#endif

#ifdef EADDRNOTAVAIL
	{STRKEY("EADDRNOTAVAIL"), INT(EADDRNOTAVAILValue)},
#endif

#ifdef EAFNOSUPPORT
	{STRKEY("EAFNOSUPPORT"), INT(EAFNOSUPPORTValue)},
#endif

#ifdef EAGAIN
	{STRKEY("EAGAIN"), INT(EAGAINValue)},
#endif

#ifdef EALREADY
	{STRKEY("EALREADY"), INT(EALREADYValue)},
#endif

#ifdef EBADF
	{STRKEY("EBADF"), INT(EBADFValue)},
#endif

#ifdef EBADMSG
	{STRKEY("EBADMSG"), INT(EBADMSGValue)},
#endif

#ifdef EBUSY
	{STRKEY("EBUSY"), INT(EBUSYValue)},
#endif

#ifdef ECANCELED
	{STRKEY("ECANCELED"), INT(ECANCELEDValue)},
#endif

#ifdef ECHILD
	{STRKEY("ECHILD"), INT(ECHILDValue)},
#endif

#ifdef ECONNABORTED
	{STRKEY("ECONNABORTED"), INT(ECONNABORTEDValue)},
#endif

#ifdef ECONNREFUSED
	{STRKEY("ECONNREFUSED"), INT(ECONNREFUSEDValue)},
#endif

#ifdef ECONNRESET
	{STRKEY("ECONNRESET"), INT(ECONNRESETValue)},
#endif

#ifdef EDEADLK
	{STRKEY("EDEADLK"), INT(EDEADLKValue)},
#endif

#ifdef EDESTADDRREQ
	{STRKEY("EDESTADDRREQ"), INT(EDESTADDRREQValue)},
#endif

#ifdef EDOM
	{STRKEY("EDOM"), INT(EDOMValue)},
#endif

#ifdef EDQUOT
	{STRKEY("EDQUOT"), INT(EDQUOTValue)},
#endif

#ifdef EEXIST
	{STRKEY("EEXIST"), INT(EEXISTValue)},
#endif

#ifdef EFAULT
	{STRKEY("EFAULT"), INT(EFAULTValue)},
#endif

#ifdef EFBIG
	{STRKEY("EFBIG"), INT(EFBIGValue)},
#endif

#ifdef EHOSTUNREACH
	{STRKEY("EHOSTUNREACH"), INT(EHOSTUNREACHValue)},
#endif

#ifdef EIDRM
	{STRKEY("EIDRM"), INT(EIDRMValue)},
#endif

#ifdef EILSEQ
	{STRKEY("EILSEQ"), INT(EILSEQValue)},
#endif

#ifdef EINPROGRESS
	{STRKEY("EINPROGRESS"), INT(EINPROGRESSValue)},
#endif

#ifdef EINTR
	{STRKEY("EINTR"), INT(EINTRValue)},
#endif

#ifdef EINVAL
	{STRKEY("EINVAL"), INT(EINVALValue)},
#endif

#ifdef EIO
	{STRKEY("EIO"), INT(EIOValue)},
#endif

#ifdef EISCONN
	{STRKEY("EISCONN"), INT(EISCONNValue)},
#endif

#ifdef EISDIR
    	{STRKEY("EISDIR"), INT(EISDIRValue)},
#endif

#ifdef ELOOP
	{STRKEY("ELOOP"), INT(ELOOPValue)},
#endif

#ifdef EMFILE
    	{STRKEY("EMFILE"), INT(EMFILEValue)},
#endif

#ifdef EMLINK
    	{STRKEY("EMLINK"), INT(EMLINKValue)},
#endif

#ifdef EMSGSIZE
    	{STRKEY("EMSGSIZE"), INT(EMSGSIZEValue)},
#endif

#ifdef EMULTIHOP
    	{STRKEY("EMULTIHOP"), INT(EMULTIHOPValue)},
#endif

#ifdef ENAMETOOLONG
    	{STRKEY("ENAMETOOLONG"), INT(ENAMETOOLONGValue)},
#endif

#ifdef ENETDOWN
    	{STRKEY("ENETDOWN"), INT(ENETDOWNValue)},
#endif

#ifdef ENETRESET
    	{STRKEY("ENETRESET"), INT(ENETRESETValue)},
#endif

#ifdef ENETUNREACH
    	{STRKEY("ENETUNREACH"), INT(ENETUNREACHValue)},
#endif

#ifdef ENFILE
    	{STRKEY("ENFILE"), INT(ENFILEValue)},
#endif

#ifdef ENOBUFS
    	{STRKEY("ENOBUFS"), INT(ENOBUFSValue)},
#endif

#ifdef ENODATA
    	{STRKEY("ENODATA"), INT(ENODATAValue)},
#endif

#ifdef ENODEV
    	{STRKEY("ENODEV"), INT(ENODEVValue)},
#endif

#ifdef ENOENT
    	{STRKEY("ENOENT"), INT(ENOENTValue)},
#endif

#ifdef ENOEXEC
    	{STRKEY("ENOEXEC"), INT(ENOEXECValue)},
#endif

#ifdef ENOLCK
   	{STRKEY("ENOLCK"), INT(ENOLCKValue)},
#endif

#ifdef ENOLINK
    	{STRKEY("ENOLINK"), INT(ENOLINKValue)},
#endif

#ifdef ENOMEM
    	{STRKEY("ENOMEM"), INT(ENOMEMValue)},
#endif

#ifdef ENOMSG
    	{STRKEY("ENOMSG"), INT(ENOMSGValue)},
#endif

#ifdef ENOPROTOOPT
    	{STRKEY("ENOPROTOOPT"), INT(ENOPROTOOPTValue)},
#endif

#ifdef ENOSPC
    	{STRKEY("ENOSPC"), INT(ENOSPCValue)},
#endif

#ifdef ENOSR
    	{STRKEY("ENOSR"), INT(ENOSRValue)},
#endif

#ifdef ENOSTR
    	{STRKEY("ENOSTR"), INT(ENOSTRValue)},
#endif

#ifdef ENOSYS
    	{STRKEY("ENOSYS"), INT(ENOSYSValue)},
#endif

#ifdef ENOTCONN
    	{STRKEY("ENOTCONN"), INT(ENOTCONNValue)},
#endif

#ifdef ENOTDIR
    	{STRKEY("ENOTDIR"), INT(ENOTDIRValue)},
#endif

#ifdef ENOTEMPTY
    	{STRKEY("ENOTEMPTY"), INT(ENOTEMPTYValue)},
#endif

#ifdef ENOTRECOVERABLE
    	{STRKEY("ENOTRECOVERABLE"), INT(ENOTRECOVERABLEValue)},
#endif

#ifdef ENOTSOCK
    	{STRKEY("ENOTSOCK"), INT(ENOTSOCKValue)},
#endif

#ifdef ENOTSUP
    	{STRKEY("ENOTSUP"), INT(ENOTSUPValue)},
#endif

#ifdef ENOTTY
    	{STRKEY("ENOTTY"), INT(ENOTTYValue)},
#endif

#ifdef ENXIO
    	{STRKEY("ENXIO"), INT(ENXIOValue)},
#endif

#ifdef EOPNOTSUPP
    	{STRKEY("EOPNOTSUPP"), INT(EOPNOTSUPPValue)},
#endif

#ifdef EOVERFLOW
    	{STRKEY("EOVERFLOW"), INT(EOVERFLOWValue)},
#endif

#ifdef EOWNERDEAD
    	{STRKEY("EOWNERDEAD"), INT(EOWNERDEADValue)},
#endif

#ifdef EPERM
    	{STRKEY("EPERM"), INT(EPERMValue)},
#endif

#ifdef EPIPE
    	{STRKEY("EPIPE"), INT(EPIPEValue)},
#endif

#ifdef EPROTO
    	{STRKEY("EPROTO"), INT(EPROTOValue)},
#endif

#ifdef EPROTONOSUPPORT
    	{STRKEY("EPROTONOSUPPORT"), INT(EPROTONOSUPPORTValue)},
#endif

#ifdef EPROTOTYPE
    	{STRKEY("EPROTOTYPE"), INT(EPROTOTYPEValue)},
#endif

#ifdef ERANGE
    	{STRKEY("ERANGE"), INT(ERANGEValue)},
#endif

#ifdef EROFS
   	{STRKEY("EROFS"), INT(EROFSValue)},
#endif

#ifdef ESPIPE
    	{STRKEY("ESPIPE"), INT(ESPIPEValue)},
#endif

#ifdef ESRCH
    	{STRKEY("ESRCH"), INT(ESRCHValue)},
#endif

#ifdef ESTALE
    	{STRKEY("ESTALE"), INT(ESTALEValue)},
#endif

#ifdef ETIME
    	{STRKEY("ETIME"), INT(ETIMEValue)},
#endif

#ifdef ETIMEDOUT
    	{STRKEY("ETIMEDOUT"), INT(ETIMEDOUTValue)},
#endif

#ifdef ETXTBSY
    	{STRKEY("ETXTBSY"), INT(ETXTBSYValue)},
#endif

#ifdef EWOULDBLOCK
    	{STRKEY("EWOULDBLOCK"), INT(EWOULDBLOCKValue)},
#endif

#ifdef EXDEV
    	{STRKEY("EXDEV"), INT(EXDEVValue)},
#endif
	{NILKEY, NILVAL}
};

#endif /* #if ((PICOC_OPTIMIZE_MEMORY == 2) && !defined (BUILTIN_MINI_STDLIB)) */
